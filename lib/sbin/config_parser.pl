#! /usr/bin/perl -s
#/*@@
#  @file      config_parser.pl
#  @date      Sep 1998
#  @author    Tom Goodale
#  @desc 
#  Parses the the configuration files for thorns.
#  @enddesc 
#  @version $Id$
#@@*/

if(! $top)
{
  $top = `pwd`;
}

if(! $config_dir)
{
  $config_dir = "$top/config-data";
}

# Set up the CCTK home directory
if(! $cctk_home)
{
  $cctk_home = $ENV{'CCTK_HOME'} || "$ENV{HOME}/CCTK";
  $cachome =~ s:/$::g;
}

if(! $bindings_dir)
{
  $bindings_dir = "$top/bindings";
}

$activethorns = shift(@ARGV);

if (! $activethorns) 
{
    printf "Usage: config_parser [-top=<TOP>] [-config_dir=<config directory>] [-cctk_home=<CCTK home dir>] -bindings_dir=<CCTK bindings directory> ActiveThornList";
    exit;
}

$sbin_dir = "$cctk_home/lib/sbin";

if (!-e "$sbin_dir/parameter_parser.pl" ) 
{
  die "Unable to find CCTK sbin directory - tried $sbin_dir\n";
}

require "$sbin_dir/parameter_parser.pl";
require "$sbin_dir/interface_parser.pl";
require "$sbin_dir/create_c_stuff.pl";
require "$sbin_dir/create_fortran_stuff.pl";
require "$sbin_dir/GridFuncStuff.pl";
require "$sbin_dir/output_config.pl";

%thorns = &create_thorn_list($cctk_home, $activethorns);

%interface_database = &create_interface_database(%thorns);

%parameter_database = &create_parameter_database(%thorns);

#&print_parameter_database(%parameter_database);

#&print_interface_database(%interface_database);


#%public_parameters = &get_public_parameters(%parameter_database);

#foreach $param (keys %public_parameters)
#{
#  print "param $param from " . $public_parameters{"$param"}. "\n";
#}

#@c_structures = &create_c_parameter_structures(scalar(@implementations),@implementations,%parameter_database);

#foreach $line (@c_structures)
#{
#  print "$line\n";
#}

#@subroutine = &create_c_param_init_subroutine("test2", %parameter_database);




#foreach $line (@subroutine)
#{
#  print "$line\n";
#}

#@GFstuff = &CreateGroups(%interface_database);

#foreach $line (@GFstuff)
#{
#  print "$line\n";
#}


&CreateBindings($bindings_dir, scalar(keys %parameter_database), %parameter_database, %interface_database);

@make_thornlist = &CreateMakeThornlist(%thorns);

&OutputFile($config_dir, "make.thornlist", @make_thornlist);

sub create_thorn_list
{
  local($cctk_home, $activethorns) = @_;
  local(%thornlist);
  local($thorn, $toolkit, $thorn_name);

  open(ACTIVE, "<$activethorns") || die "Cannot open ActiveThorns file $activethorns !";

  $thornlist{"Cactus"} = "$cctk_home/src";

  print "cctk_home is $cctk_home\n";


  while(<ACTIVE>)
  {
    s/\#(.*)$//g;
    s/\n//g;		# Different from chop...
    next if (m:^\s*$:);
    foreach $thorn (split(' '))
    {
      $thorn =~ m:(.*)[/\\](.*):;

      $toolkit = $1;
      $thorn_name = $2;
      $thorn_name =~ s/thorn_//;
      
      if( -r "$cctk_home/toolkits/$thorn/param.ccl" &&
	  -r "$cctk_home/toolkits/$thorn/interface.ccl" &&
	  -r "$cctk_home/toolkits/$thorn/schedule.ccl")
      {
	if( $thornlist{"$thorn_name"} )
	{
	  print "Ignoring duplicate thorn $thorn_name\n";
	}
	else
	{
	  $thornlist{"$thorn_name"} = "$cctk_home/toolkits/$thorn";
	}
      }
      else
      {
	print "Ignoring $thorn - missing ccl file(s)\n";
	next;
      }
    }
  }
  close ACTIVE;

  return %thornlist;
}


sub get_public_parameters
{
  local(%parameter_database) = @_;
  local(%public_parameters);
  local($param);

  foreach $param (split(/ /,$parameter_database{"PUBLIC PARAMETERS"}))
  {
    if($param =~ m/(.*)::(.*)/)
    {
      $public_parameters{"$2"} = $1;
    }
  }

  return %public_parameters;
}

    
sub CreateMakeThornlist
{
  local(%thorns) = @_;
  local($thorn);
  local($thornlist);

  $thornlist = "THORNS =";
  foreach $thorn (keys %thorns)
  {
    next if ($thorn =~ m:Cactus:);

    $thorns{$thorn} =~ m:(.*)/(.*)/(.*):;

    $thornlist .= " $2/$3";
  }

  return ("$thornlist", "");
}
  
sub CreateBindings
{
  local($bindings_dir, $n_param_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local($start_dir);

  %parameter_database = @rest[0..2*$n_param_database-1];
  %interface_database = @rest[2*$n_param_database..$#rest];

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;

  &CreateParameterBindings($bindings_dir, $n_param_database, @rest);
  &CreateVariableBindings($bindings_dir, %interface_database);
  &CreateScheduleBindings($bindings_dir);

  chdir $bindings_dir;

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  print OUT "SRCS = \n";
  print OUT "SUBDIRS = Parameters Variables Schedule\n";

  close OUT;
    
  chdir $start_dir;
  
}

sub CreateParameterBindings
{
  local($bindings_dir, $n_param_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local($start_dir);
  local($line);
  
  %parameter_database = @rest[0..(2*$n_param_database)-1];
  %interface_database = @rest[2*$n_param_database..$#rest];

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;

  chdir $bindings_dir;

  if(! -d "Parameters")
  {
    mkdir("Parameters", 0755) || die "Unable to create Parameters directory";
  }
  chdir "Parameters";

  @data = &CreatePublicParamBindings("CCTK_BindingsParametersPublic", $n_param_database, @rest);

  open (OUT, ">Public.c") || die "Cannot open Public.c";

  foreach $line (@data)
  {
    print OUT "$line\n";
  }

  close OUT;

  open (OUT, ">Bindings.c") || die "Cannot open Bindings.c";

  print OUT  <<EOT;
 
  int CCTK_BindingsParametersInitialise(void)
  {
    return 0;
  }
 
  int CCTK_BindingsParameterSet(const char *identifier, const char *value)
  {
    return 1;
  }
 
  int CCTK_BindingsParameterGet(const char *identifier, void **value)
  {
    return -1;
  }
 
EOT

  close OUT;

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  print OUT "SRCS = Bindings.c\n";

  close OUT;
    
  chdir $start_dir;
}


sub CreateVariableBindings
{
  local($bindings_dir, %interface_database) = @_;

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;
  chdir $bindings_dir;

  if(! -d "Variables")
  {
    mkdir("Variables", 0755) || die "Unable to create Variables directory";
  }
  chdir "Variables";

  open (OUT, ">Bindings.c") || die "Cannot open Bindings.c";

  print OUT  <<EOT;
 
  int CCTK_BindingsVariablesInitialise(void)
  {
    return 0;
  }
 
EOT

  close OUT;

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  print OUT "SRCS = Bindings.c\n";

  close OUT;

  chdir $start_dir;
}

sub CreateScheduleBindings
{
  local($bindings_dir) = @_;

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;
  chdir $bindings_dir;

  if(! -d "Schedule")
  {
    mkdir("Schedule", 0755) || die "Unable to create Schedule directory";
  }
  chdir "Schedule";

  open (OUT, ">Bindings.c") || die "Cannot open Bindings.c";

  print OUT  <<EOT;
 
  int CCTK_BindingsScheduleInitialise(void)
  {
    return 0;
  }

  int CCTK_BindingsScheduleRegister(const char *type, void *data)
  {
    return 0;
  }
 
EOT

  close OUT;

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  print OUT "SRCS = Bindings.c\n";

  close OUT;

  chdir $start_dir;
}



sub CreatePublicParamBindings
{
  local($prefix, $n_param_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local($line,@data);
  local(%public_parameters);
  local($type, $type_string);
  local(@data);

  %parameter_database = @rest[0..2*$n_param_database-1];
  %interface_database = @rest[2*$n_param_database..$#rest];

  %public_parameters = &get_public_parameters(%parameter_database);

  # Header Data
  $line = "\#include <stdio.h>";
  push(@data, $line);
  $line = "\#include <stdlib.h>";
  push(@data, $line);
  $line = "\#include <string.h>";
  push(@data, $line);
  $line = "\#include <stdarg.h>";
  push(@data, $line);
  $line = "\#include \"Misc.h\"";
  push(@data, $line);
  push(@data, "");

  # Create the structure

  push(@data,( "struct ", "{"));

  foreach $parameter (keys %public_parameters)
  {
    $type = $parameter_database{"\U$public_parameters{$parameter} $parameter\E type"};
      
    $type_string = &get_c_type_string($type);
    
    $line = $type_string ." " .$parameter . ";";

    push(@data, $line);
  }

  push(@data, "} PUBLIC_PARAM_STRUCT;");

  push(@data, "");

  # Initialisation subroutine
  push(@data, ("int $prefix"."Initialise(void)", "{"));

  foreach $parameter (keys %public_parameters)
  {

    push(@data, &set_parameter_default("PUBLIC_PARAM_STRUCT",$public_parameters{$parameter}, 
				       $parameter, %parameter_database));
    
    push(@data, "");

  }

  push(@data, "}");

  push(@data, "");

  # Setting subroutine

  push(@data, ("int $prefix"."Set(const char *param, const char *value)", "{"));
  push(@data, ("  char temp[1001];", "  int p;", ""));

  push(@data, ("  int retval;", "  retval = 1;", ""));


  foreach $parameter (keys %public_parameters)
  {
    push(@data, &set_parameter_code("PUBLIC_PARAM_STRUCT",$public_parameters{$parameter}, 
				       $parameter, %parameter_database));
    push(@data, "");

  }    

  push(@data, "  return retval;");

  push(@data, "}");

  push(@data, "");


  return @data;
}


sub set_parameter_code
{
  local($structure, $implementation,$parameter, %parameter_database) = @_;
  local($type, $type_string);
  local($line, @lines);
  local($range);
  local($quoted_range);

  $type = $parameter_database{"\U$implementation $parameter\E type"};
  $n_ranges = $parameter_database{"\U$implementation $parameter\E ranges"};

  push(@lines,("  if(CCTK_Equals(param, \"$parameter\"))", "  {"));

  if( $type ne "STRING" && $type ne "SENTENCE" && $type ne "LOGICAL")
  {
    if( $type eq "KEYWORD")
    {
      $line = "    if(CCTK_InList(value, $n_ranges" ;
    }
    elsif($type eq "INTEGER")
    {
      $line = "    if(CCTK_IntInRangeList(atoi(value), $n_ranges" ;
    }
    elsif($type eq "REAL")
    {
      $line = "    strncpy(temp, value, 1000);";
      push(@lines, $line);

      $line = "    for (p=0;p<strlen(temp);p++) if (temp[p] == 'E' || temp[p] == 'd' || temp[p] == 'D') temp[p] = 'e';";
      push(@lines, $line);
      $line = "    if(CCTK_DoubleInRangeList(atof(temp), $n_ranges" ;
    }
    for($range=1; $range <= $n_ranges; $range++)
    {
      $quoted_range = $parameter_database{"\U$implementation $parameter\E range $range range"};

      $quoted_range =~ s:\":\\\":g;

      $line .= ",\"".$quoted_range."\"";

    }
    $line .= "))";

    push(@lines, ($line, "    {"));

    if( $type eq "KEYWORD")
    {
      $line = "      if($structure.$parameter) free($structure.$parameter);";
      push(@lines, $line);

      $line = "      $structure" .".$parameter = malloc(strlen(value)\*sizeof(char));"; 
      push(@lines, $line);
      
      $line = "  if($structure.$parameter)";
      push(@lines, $line);
      
      $line = "    strcpy($structure.$parameter, value);";
      push(@lines, ($line, "         retval = 0;", "    }"));
      
    }
    elsif($type eq "INTEGER")
    {
      $line = "      $structure.$parameter = atoi(value);" ;
      push(@lines, ($line, "         retval = 0;", "    }"));
    }
    elsif($type eq "REAL")
    {
      push(@lines, "         $structure.$parameter = atof(temp); ");

      push(@lines, ($line, "         retval = 0;", "    }"));

    }

    push(@lines, "  }");

  }
  elsif( $type eq "STRING" || $type eq "SENTENCE")
  {
    $line = "      if($structure.$parameter) free($structure.$parameter);";
    push(@lines, $line);
    
    $line = "      $structure" .".$parameter = malloc(strlen(value)\*sizeof(char));"; 
    push(@lines, $line);
      
    $line = "      if($structure.$parameter)";
    push(@lines, $line);
      
    $line = "        strcpy($structure.$parameter, value);";
    push(@lines, ($line, "  }"));
  }
  elsif( $type eq "LOGICAL")
  {
    push(@lines, ("    if(CCTK_InList(value, 4, \"true\", \"t\", \"yes\", \"1\"))"," {", "$structure.$parameter = 1", "}", "else if(CCTK_InList(value, 4, \"false\", \"f\", \"no\", \"0\"))"," {", "$structure.$parameter = 0", "}", "else", "{ ", "retval = 2" , "};"));
  }
  else
  {
    print "Unknown parameter type $type\n";
  }


  return @lines;
}

    

#/*@@
#  @routine    set_parameter_default
#  @date       Mon Jan 11 15:33:26 1999
#  @author     Tom Goodale
#  @desc 
#  Set the default value of a parameter
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub set_parameter_default
{
  local($structure, $implementation,$parameter, %parameter_database) = @_;
  local($type, $type_string);
  local($line, @lines);
  local($default);

  $default = $parameter_database{"\U$implementation $parameter\E default"};
  $type = $parameter_database{"\U$implementation $parameter\E type"};

  $type_string = &get_c_type_string($type);

  if($type_string eq "char *")
  {
    $line = "  $structure" .".$parameter = malloc(" 
      . (length($default)-1). "\*sizeof(char));";
    push(@lines, $line);

    $line = "  if($structure.$parameter)";
    push(@lines, $line);

    $line = "    strcpy($structure.$parameter, $default);";
    push(@lines, $line);
  }
  else
  {
    $line = "  $structure.$parameter = $default;";
    push(@lines, $line);
  }

  return @lines;
}

#/*@@
#  @routine    get_c_type_string
#  @date       Mon Jan 11 15:33:50 1999
#  @author     Tom Goodale
#  @desc 
#  Returns the correct type string for a parameter
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

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
