#! /usr/bin/perl

$in = shift(@ARGV);
$tmphome = shift(@ARGV);

if (!$in || !$tmphome) 
{
    printf "Usage: xxx indir tmpdir";
}

# Set up the CCTK home directory
$cachome = $ENV{'CCTK_HOME'} || "$ENV{HOME}/CCTK";
$cachome =~ s:/$::g;

#
#if (!-e "$cachome/lib/perl/thorn_utils.pl" ) {
#    print <<EOE;
#
#ERROR: Cannot find the cactus perl libraries!
#---------------------------------------------
#This error usually means that either CACTUS_HOME is
#set incorrectly, or that your distribution is not
#in ~/cactus. Remeber that CACTUS_HOME has
#to point to the directory above cactus. 
#
#I was looking in
#
#    $cachome/lib/perl
#  
#when I failed.
#
#
#EOE
#    exit;
#}


require "lib/sbin/parameter_parser.pl";
require "lib/sbin/interface_parser.pl";

%thorns = &create_thorn_list;

%interface_database = create_interface_database(%thorns);

%parameter_database = create_parameter_database(%thorns);


@parameter_structure = &create_c_parameter_structure(%parameter_database);


#foreach $line (@parameter_structure)
#{
#  print "$line\n";
#}

@c_parameter_declarations = &create_c_parameter_declarations("test2",%parameter_database);

#foreach $line (@c_parameter_declarations)
#{
#  print "$line\n";
#}

#&print_parameter_database(%parameter_database);

#&print_interface_database(%interface_database);

@fortran_parameter_declarations = &create_fortran_parameter_declarations("test2",%parameter_database);

#foreach $line (@fortran_parameter_declarations)
#{
#  print "$line\n";
#}

@fortran_parameter_structures = &create_fortran_parameter_structures(%parameter_database);

#foreach $line (@fortran_parameter_structure)
#{
#  print "$line\n";
#}


@fortran_module = &create_fortran_param_module("test2",%parameter_database);

#foreach $line (@fortran_module)
#{
#  print "$line\n";
#}

@fortran_subroutine = &create_fortran_param_init_subroutine("test2",%parameter_database);

#foreach $line (@fortran_subroutine)
#{
#  print "$line\n";
#}

@implementations = (keys %thorns);

@fortran_module_file = &create_fortran_module_file(scalar(@implementations),@implementations,%parameter_database);

foreach $line (@fortran_module_file)
{
  print "$line\n";
}


sub create_fortran_module_file
{
  local($n_implementations, @indata) = @_;
  local(@implementations);
  local(%parameter_database);
  local(@module_file);
  local($line);
  local(@data);

  @implementations = @indata[0..$n_implementations-1];
  %parameter_database = @indata[$n_implementations..$#indata];

  push(@module_file, &create_cctk_param_types_module($n_implementations, @indata));


  $line = "";
  push(@module_file, $line);

  foreach $implementation (@implementations)
  {
    $line = "";
    push(@module_file, $line);

    push(@module_file, &create_fortran_param_module($implementation, %parameter_database));

    $line = "";
    push(@module_file, $line);
  }

  $line = "SUBROUTINE CCTK_PARAMS_INIT(cctk_parameters)";
  push(@module_file, $line);


  $line = "  USE CCTK_PARAM_TYPES";
  push(@module_file, $line);

  $line = "  TYPE (CCTK_PARAMS) :: cctk_parameters";
  push(@module_file, $line);
  

  foreach $implementation (@implementations)
  {
    $line = "";
    push(@module_file, $line);

    $line = "  CALL CCTK_\U$implementation\E_PARAMS_INIT(cctk_parameters)";
    push(@module_file, $line);

    $line = "";
    push(@module_file, $line);
  }

  $line = "END SUBROUTINE CCTK_PARAMS_INIT";
  push(@module_file, $line);

  $line = "";
  push(@module_file, $line);
  
  foreach $implementation (@implementations)
  {
    $line = "";
    push(@module_file, $line);

    push(@module_file, &create_fortran_param_init_subroutine($implementation, %parameter_database));

    $line = "";
    push(@module_file, $line);
  }
  
  return @module_file;

}
    

sub create_fortran_param_init_subroutine
{
  local($implementation,%parameter_database) = @_;
  local(@subroutine);
  local($line);
  local($friend, $block, $parameter);
    

  $line = "SUBROUTINE CCTK_\U$implementation\E_PARAMS_INIT(cctk_parameters)";
  push(@subroutine, $line);

  $line = "  USE CCTK_PARAM_TYPES";
  push(@subroutine, $line);

  $line = "  USE CCTK_\U$implementation\E_PARAMS";
  push(@subroutine, $line);
  
  $line = "  TYPE (CCTK_PARAMS) :: cctk_parameters";
  push(@subroutine, $line);
  

  # Deal with variables defined in this thorn. 
  foreach $block ("PUBLIC", "PRIVATE", "PROTECTED")
  {
    $entry = "\U$implementation $block\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      
      $line = "  $parameter" . 
	" = cctk_parameters%" . "\U$implementation\E%\L$parameter\E";
      push(@subroutine, $line);
    }
  }

  # Deal with friend variables. 
  foreach $friend (split(/ /,$parameter_database{"\U$implementation\E FRIEND implementations"}))
  {
    $other_implementation = "\U$friend\E";
    $entry = "\U$implementation FRIEND $friend\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $line = "  $parameter" . 
	" = cctk_parameters%" . "\U$other_implementation\E%\L$parameter\E";
      push(@subroutine, $line);
    }
  }	  
  
  
  $line = "END SUBROUTINE CCTK_\U$implementation\E_PARAMS_INIT";
  push(@subroutine, $line);

  return @subroutine;
}



sub create_fortran_param_module
{
  local($implementation,%parameter_database) = @_;
  local(@declarations);
  local(@module);
  local($line);

  $line = "MODULE CCTK_\U$implementation\E_PARAMS";
  push(@module, $line);

  @declarations = &create_fortran_parameter_declarations($implementation,%parameter_database);
  
  foreach $line (@declarations)
  {
    $line = "  " . $line;
    push(@module, $line);
  }
  
  $line = "END MODULE CCTK_\U$implementation\E_PARAMS";
  push(@module, $line);

  return @module;
}
    

sub create_cctk_param_types_module
{
  local(@module);

  push(@module, "MODULE CCTK_PARAM_TYPES");
  push(@module, &create_fortran_parameter_structures(@_));
  push(@module, "END MODULE CCTK_PARAM_TYPES");

  return @module;
}


sub create_fortran_parameter_structures
{
  local($n_implementations, @indata) = @_;
  local(@implementations);
  local(%parameter_database);
  local(@structures);
  local(@data);
  local($line, $entry, $thorn, $parameter, $type_string);

  @implementations = @indata[0..$n_implementations-1];
  %parameter_database = @indata[$n_implementations..$#indata];

  # Create types for each implementation
  foreach $implementation (@implementations)
  {
    push(@structures, &create_fortran_parameter_type_declaration($implementation, %parameter_database));
    push(@strucures,"");
  }

  $line = "TYPE CCTK_PARAMS ";
  push(@structures, $line);

  foreach $implementation (@implementations)
  {
    $line = "TYPE (CCTK_\U$implementation\E_PARAM_TYPE) :: \U$implementation\E";
    push(@structures, $line);
  }

  $line = "END TYPE CCTK_PARAMS";
  push(@structures, $line);
  
  return @structures;
}

sub create_fortran_parameter_declarations
{
  local($implementation,%parameter_database) = @_;
  local(@declarations);
  local($line);
  local($type, $type_string, $friend, $block, $parameter);
  
  # Deal with variables defined in this thorn. 
  foreach $block ("PUBLIC", "PRIVATE", "PROTECTED")
  {
    $entry = "\U$implementation $block\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = @parameter_database{"\U$implementation $parameter\E type"};
      
      $type_string = &get_fortran_type_string($type);
      
      $line = $type_string .", save :: " .$parameter ;
      push(@declarations, $line);
    }
  }

  # Deal with friend variables. 
  foreach $friend (split(/ /,$parameter_database{"\U$implementation\E FRIEND implementations"}))
  {
    $other_implementation = "\U$friend\E";
    $entry = "\U$implementation FRIEND $friend\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = @parameter_database{"\U$other_implementation $parameter\E type"};
      
      $type_string = &get_fortran_type_string($type);
      
      $line = $type_string .", save :: " .$parameter ;
      push(@declarations, $line);
    }
  }	  
  return @declarations;
}

sub create_c_parameter_declarations
{
  local($implementation,%parameter_database) = @_;
  local(@declarations);
  local($line);
  local($type, $type_string, $friend, $block, $parameter);

  # Deal with variables defined in this thorn. 
  foreach $block ("PUBLIC", "PRIVATE", "PROTECTED")
  {
    $entry = "\U$implementation $block\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = @parameter_database{"\U$implementation $parameter\E type"};
      
      $type_string = &get_c_type_string($type);
      
      $line = "  ". $type_string .$parameter . 
	" = _cctk_params." . "\U$implementation\E_\L$parameter\E;";
      push(@declarations, $line);
    }
  }

  # Deal with friend variables. 
  foreach $friend (split(/ /,$parameter_database{"\U$implementation\E FRIEND implementations"}))
  {
    $other_implementation = "\U$friend\E";
    $entry = "\U$implementation FRIEND $friend\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = @parameter_database{"\U$other_implementation $parameter\E type"};
      
      $type_string = &get_c_type_string($type);
      
      $line = "  ". $type_string .$parameter . 
	" = _cctk_params." . "\U$other_implementation\E_\L$parameter\E;";
      push(@declarations, $line);
    }
  }	  
  return @declarations;
}

sub create_c_parameter_structure
{
  local(%parameter_database) = @_;
  local(@structure);
  local($line, $entry, $thorn, $parameter, $type_string);

  $line = "struct CCTK_PARAMS {";

  push(@structure, $line);

  foreach $entry ( keys %parameter_database )
  {
    if($entry =~ m:([^ ]*) ([^ ]*) type:)
    {
      $thorn = $1;
      $parameter = "\L$2\E";
      $type_string = &get_c_type_string($parameter_database{$entry});

      $line = "  ". $type_string . $thorn . "_". $parameter .";";
      push(@structure, $line);
    }
  }

  $line = "};";
  push(@structure, $line);
  
  return @structure;
}
      
sub create_thorn_list
{
  return ("flesh", "toolkits/test/flesh", 
	   "test1", "toolkits/test/test1", 
	   "test2", "toolkits/test/test2");
}

sub get_c_type_string
{
  local($type) = @_;
  local($type_string);


  if($type eq "KEYWORD" ||
     $type eq "STRING"  ||
     $type eq "SENTENCE")
  {
    $type_string = "char *";
  }
  elsif($type eq "LOGICAL" ||
	$type eq "INTEGER")
  {
    $type_string = "int ";
  }
  elsif($type eq "REAL")
  {
    $type_string = "Double ";
  }
  else
  {
    die("Unknown parameter type '$type'");
  }

  return $type_string;

}

sub get_fortran_type_string
{
  local($type) = @_;
  local($type_string);


  if($type eq "KEYWORD" ||
     $type eq "STRING"  ||
     $type eq "SENTENCE")
  {
    $type_string = "CACTUS_STRING ";
  }
  elsif($type eq "LOGICAL" ||
	$type eq "INTEGER")
  {
    $type_string = "INTEGER ";
  }
  elsif($type eq "REAL")
  {
    $type_string = "REAL ";
  }
  else
  {
    die("Unknown parameter type '$type'");
  }

  return $type_string;

}

sub create_fortran_parameter_type_declaration
{
  local($implementation,%parameter_database) = @_;
  local(@type_declaration);
  local($line);
  local($type, $type_string, $friend, $block, $parameter);
  
  $line = "TYPE CCTK_\U$implementation\E_PARAM_TYPE";
  push(@type_declaration, $line);
 
  # Deal with variables defined in this thorn. 
  foreach $block ("PUBLIC", "PRIVATE", "PROTECTED")
  {
    $entry = "\U$implementation $block\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = @parameter_database{"\U$implementation $parameter\E type"};
      
      $type_string = &get_fortran_type_string($type);
      
      $line = $type_string ." :: " .$parameter ;
      push(@type_declaration, $line);
    }
  }

  # Deal with friend variables. 
  foreach $friend (split(/ /,$parameter_database{"\U$implementation\E FRIEND implementations"}))
  {
    $other_implementation = "\U$friend\E";
    $entry = "\U$implementation FRIEND $friend\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = @parameter_database{"\U$other_implementation $parameter\E type"};
      
      $type_string = &get_fortran_type_string($type);
      
      $line = $type_string ." :: " .$parameter ;
      push(@type_declaration, $line);
    }
  }	  

  $line = "END TYPE CCTK_\U$implementation\E_PARAM_TYPE";
  push(@type_declaration, $line);

  return @type_declaration;
}
