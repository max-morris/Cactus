#! /usr/bin/perl
#/*@@
#  @file      create_fortran_stuff.pl
#  @date      Tue Jan 12 09:52:35 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc 
#@@*/

#/*@@
#  @routine    create_fortran_module_file
#  @date       Tue Jan 12 09:53:04 1999
#  @author     Tom Goodale
#  @desc 
#  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

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
    

#/*@@
#  @routine    create_fortran_param_init_subroutine
#  @date       Tue Jan 12 09:53:54 1999
#  @author     Tom Goodale
#  @desc 
#  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_fortran_param_init_subroutine
{
  local($implementation,%parameter_database) = @_;
  local(@subroutine);
  local($line);
  local($friend, $block, $parameter);
  local(%public_parameters);

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
  # Deal with all public parameters not declared in this implementation.
  %public_parameters = &get_public_parameters(%parameter_database);

  foreach $parameter (keys %public_parameters)
  {
    $other_implementation = "\U$public_parameters{$parameter}\E";
    if($other_implementation ne "\U$implementation\E")
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



sub create_fortran_parameter_declarations
{
  local($implementation,%parameter_database) = @_;
  local(@declarations);
  local($line);
  local($type, $type_string, $friend, $block, $parameter);
  local(%public_parameters);

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
	  
  # Deal with all public parameters not declared in this implementation.
  %public_parameters = &get_public_parameters(%parameter_database);

  foreach $parameter (keys %public_parameters)
  {
    $other_implementation = "\U$public_parameters{$parameter}\E";

    if($other_implementation ne "\U$implementation\E")
    {
      $type = @parameter_database{"\U$other_implementation $parameter\E type"};
    
      $type_string = &get_fortran_type_string($type);
    
      $line = $type_string .", save :: " .$parameter ;
      push(@declarations, $line);
    }
  }
  return @declarations;
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


  $line = "END TYPE CCTK_\U$implementation\E_PARAM_TYPE";
  push(@type_declaration, $line);

  return @type_declaration;
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

1;
