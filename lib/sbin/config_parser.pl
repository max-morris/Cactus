#! /usr/bin/perl -s

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

$activethorns = shift(@ARGV);

if (! $activethorns) 
{
    printf "Usage: config_parser [-top=<TOP>] [-config_dir=<config directory>] [-cctk_home=<CCTK home dir>] ActiveThornList";
    exit;
}


#if (!-e "$cctk_home/lib/perl/thorn_utils.pl" ) {
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

require "$cctk_home/lib/sbin/parameter_parser.pl";
require "$cctk_home/lib/sbin/interface_parser.pl";
require "$cctk_home/lib/sbin/create_c_stuff.pl";
require "$cctk_home/lib/sbin/create_fortran_stuff.pl";
require "$cctk_home/lib/sbin/GridFuncStuff.pl";

%thorns = &create_thorn_list($activethorns);

foreach $thorn (keys %thorns)
{
  print "$thorn in dir $thorns{$thorn}\n";
}

%interface_database = &create_interface_database(%thorns);

%parameter_database = &create_parameter_database(%thorns);

#&print_parameter_database(%parameter_database);

#&print_interface_database(%interface_database);

@implementations = (keys %thorns);

@fortran_module_file = &create_fortran_module_file(scalar(@implementations),@implementations,%parameter_database);

foreach $line (@fortran_module_file)
{
  print "$line\n";
}

#%public_parameters = &get_public_parameters(%parameter_database);

#foreach $param (keys %public_parameters)
#{
#  print "param $param from " . $public_parameters{"$param"}. "\n";
#}

@c_structures = &create_c_parameter_structures(scalar(@implementations),@implementations,%parameter_database);

foreach $line (@c_structures)
{
  print "$line\n";
}

@subroutine = &create_c_param_init_subroutine("test2", %parameter_database);




#foreach $line (@subroutine)
#{
#  print "$line\n";
#}

@GFstuff = &CreateGroups(%interface_database);

foreach $line (@GFstuff)
{
  print "$line\n";
}

      
sub create_thorn_list
{
  local($activethorns) = @_;
  local(%thornlist);
  local($thorn, $toolkit, $thorn_name);

  open(ACTIVE, "<$activethorns") || die "Cannot open ActiveThorns file $activethorns !";

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
	$thornlist{"$thorn_name"} = "$cctk_home/toolkits/$thorn";
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

    
  

    
  
