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


&CreateBindings($bindings_dir, scalar(%parameter_database), %parameter_database, %interface_database);

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
    s/thorn_//g;
    s/\#(.*)$//g;
    s/\n//g;		# Different from chop...
    next if (m:^\s*$:);
    foreach $thorn (split(' '))
    {
      $thorn =~ m:(.*)[/\\](.*):;

      $toolkit = $1;
      $thorn_name = $2;

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

  %parameter_database = @rest[0..$n_param_database-1];
  %interface_database = @rest[$n_param_database..$#rest];

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

  %parameter_database = @rest[0..$n_param_database-1];
  %interface_database = @rest[$n_param_database..$#rest];

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

  open (OUT, ">make.code.defn") || die "Cannot open ake.code.defn";

  print OUT "SRCS = Bindings.c\n";

  close OUT;
    
  chdir $start_dir;
}


sub CreateVariableBindings
{
  local($bindings_dir, %inteface_database) = @_;

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
