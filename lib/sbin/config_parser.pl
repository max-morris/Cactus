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
  local(%these_parameters);
  local($implementation, $thorn);
  local($files);
  local(%routines);

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

  if(! -d "include")
  {
    mkdir("include", 0755) || die "Unable to create include directory";
  }

  chdir "Parameters";


  # Generate all public parameters
  %these_parameters = &get_public_parameters(%parameter_database);

  @data = &CreateParameterBindingFile("CCTK_BindingsParametersPublic", "PUBLIC_PARAMETER_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

  open (OUT, ">Public.c") || die "Cannot open Public.c";

  foreach $line (@data)
  {
    print OUT "$line\n";
  }

  close OUT;

  $files = "Public.c";

  # Generate the public data header file

  chdir "..";
  chdir "include";

  @data = &CreateCStructureParameterHeader("CCTK_BindingsParametersPublic", "PUBLIC_PARAMETER_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

  open (OUT, ">ParameterCPublic.h") || die "Cannot open ParameterCPublic.h";

  foreach $line (@data)
  {
    print OUT "$line\n";
  }

  close OUT;

  chdir "..";
  chdir "Parameters";

  # Generate all protected parameters
  foreach $implementation (split(" ",$interface_database{"IMPLEMENTATIONS"}))
  {
    $interface_database{"IMPLEMENTATION \U$implementation\E THORNS"} =~ m:([^ ]+):;

    $thorn = $1;

    %these_parameters = &GetThornParameterList($thorn, "PROTECTED", %parameter_database);

    if((keys %these_parameters > 0))
    {
      @data = &CreateParameterBindingFile("CCTK_BindingsParameters$implementation"."_protected", "PROTECTED_\U$implementation\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

      open (OUT, ">$implementation". "_protected.c") || die "Cannot open $implementation"."_protected.c";
      
      foreach $line (@data)
      {
	print OUT "$line\n";
      }
    
      close OUT;

      $files .= " $implementation". "_protected.c";
      $routines{"CCTK_BindingsParameters$implementation"."_protected"} = "$implementation";

      # Generate the data header file
      
      chdir "..";
      chdir "include";

      @data = &CreateCStructureParameterHeader("CCTK_BindingsParameters$implementation"."_protected", "PROTECTED_\U$implementation\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);
      
      open (OUT, ">ParameterCProtected$implementation".".h") || die "Cannot open ParameterCProtected$implementation".".h";

      foreach $line (@data)
      {
	print OUT "$line\n";
      }

      close OUT;

      chdir "..";
      chdir "Parameters";

    }
  }

  # Generate all private parameters
  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    %these_parameters = &GetThornParameterList($thorn, "PRIVATE", %parameter_database);

    if((keys %these_parameters > 0))
    {
      @data = &CreateParameterBindingFile("CCTK_BindingsParameters$thorn"."_private", "PRIVATE_\U$thorn\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

      open (OUT, ">$thorn"."_private.c") || die "Cannot open $thorn"."_private.c";

      foreach $line (@data)
      {
	print OUT "$line\n";
      }
    
      close OUT;

      $files .= " $thorn". "_private.c";
      $routines{"CCTK_BindingsParameters$thorn"."_private"} = "$thorn";

      # Generate the data header file
      
      chdir "..";
      chdir "include";

      @data = &CreateCStructureParameterHeader("CCTK_BindingsParameters$thorn"."_private", "PRIVATE_\U$thorn\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);
      
      open (OUT, ">ParameterCPrivate$thorn".".h") || die "Cannot open ParameterCPrivate$thorn".".h";

      foreach $line (@data)
      {
	print OUT "$line\n";
      }

      close OUT;

      chdir "..";
      chdir "Parameters";

    }
  }

  open (OUT, ">Bindings.c") || die "Cannot open Bindings.c";

  print OUT  <<EOT;
#include <stdio.h>
#include "config.h"
#include "Misc.h"

EOT

  foreach $routine ((keys %routines), "CCTK_BindingsParametersPublic")
  {
    print OUT "int $routine"."Initialise(void);\n";
    print OUT "int $routine"."Set(const char *param, const char *value);\n";
    print OUT "int $routine"."Get(const char *param, void **data);\n";
  }
 
print OUT <<EOT;

int CCTK_BindingsParametersInitialise(void)
{

EOT

  foreach $routine (keys %routines)
  {
    print OUT "  $routine"."Initialise();\n";
  }
  print OUT <<EOT;
  return 0;
}
 
int CCTK_BindingsParameterSet(const char *identifier, const char *value)
{
  int retval = 1;
  int temp_retval;
  char *implementation = NULL;
  char *param_name = NULL;

  CCTK_SplitString(&implementation, &param_name, identifier, "::");

  if(!implementation)
  {
    retval = CCTK_BindingsParametersPublicSet(identifier, value);
  }
  else
  { 
EOT

  foreach $routine (keys %routines)
  {

    print OUT <<EOT;

    if(CCTK_Equals(implementation, \"$routines{$routine}\"))
    {
EOT
      print OUT "      temp_retval =  $routine"."Set(param_name, value);";

    print OUT <<EOT;
 
      if(!temp_retval) 
      {
        retval = 0;
      }
    }
EOT
  }
 
  print OUT <<EOT;
  }
 
  free(implementation);
  free(param_name);
  return retval;
}
 
int CCTK_BindingsParameterGet(const char *identifier, void **value)
{   
  return -1;
}
 
EOT
  
  close OUT;

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  print OUT "SRCS = Bindings.c $files\n";

  close OUT;

  # Create the appropriate thorn parameter headers

  chdir "..";
  chdir "include";

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {

    open(OUT, ">$thorn"."_CParameters.h") || die "Cannot open $thorn"."_CParameters.h";

    $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};

    print OUT <<EOT;
 
\#ifndef _\U$thorn\E_PARAMETERS_H_
 
\#define _\U$thorn\E_PARAMETERS_H_

EOT

  $header =  "ParameterCPublic.h";
  if( -r $header)
  {
    print OUT "#include \"$header\"\n";
  }
 
  $header =  "ParameterCProtected$implementation.h";
  if( -r $header)
  {
    print OUT "#include \"$header\"\n";
  }

  $header =  "ParameterCPrivate$thorn.h";
  if( -r $header)
  {
    print OUT "#include \"$header\"\n";
  }
 
  print OUT "\n";

    @data = ();
    foreach $friend (split(" ",$parameter_database{"\U$thorn\E FRIEND implementations"}))
    {
      $friend_implementation = $interface_database{"\U$friend\E IMPLEMENTS"};

      print OUT "#include \"ParameterCProtected$friend_implementation.h\"\n";

      foreach $parameter (split(" ",$parameter_database{"\U$thorn FRIEND $friend\E variables"}))
      {
	$interface_database{"IMPLEMENTATION \U$friend\E THORNS"} =~ m:([^ ]*):;
 
	$friend_thorn = $1;

	$type = $parameter_database{"\U$friend_thorn $parameter\E type"};

	$type_string = &get_c_type_string($type);

	$line = "  ".$type_string ." " .$parameter . " = PROTECTED_\U$friend\E_STRUCT.$parameter;";

	push(@data, $line);

      }
    }

    print OUT "#define DECLARE_PARSER \\\n";

    $header =  "ParameterCPublic.h";
    $decl =  "DECLARE_PUBLIC_PARAMETER_STRUCT_PARAMS";
    if( -r $header)
    {
      print OUT "$decl \\\n";
    }
 
    $header =  "ParameterCProtected$implementation.h";
    $decl = "DECLARE_PROTECTED_\U$implementation\E_STRUCT_PARAMS";
    if( -r $header)
    {
      print OUT "$decl \\\n";
    }

    $header =  "ParameterCPrivate$thorn.h";
    $decl = "DECLARE_PRIVATE_\U$thorn\E_STRUCT_PARAMS";
    if( -r $header)
    {
      print OUT "$decl \\\n";
    }

    foreach $line (@data)
    {
      print OUT $line . "\\\n";
    }

    print OUT "\n";

    print OUT "#endif\n";

    close OUT;
  }   
    
  open(OUT, ">CParameters.h") || die "Cannot open CParameters.h";

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "#ifdef THORN_IS_$thorn\n";
    print OUT "#include \"$thorn"."_CParameters.h\"\n";
    print OUT "#endif\n\n";
  }

  close OUT;

  open(OUT, ">cctk_parameters.h") || die "Cannot open cctk_parameters.h";

  print OUT "#ifdef CCODE\n";
  print OUT "#include \"CParameters.h\"\n";
  print OUT "#endif\n\n";


  print OUT "#ifdef FCODE\n";
  print OUT "#include \"FParameters.h\"\n";
  print OUT "#endif\n\n";

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



