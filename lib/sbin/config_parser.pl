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
require "lib/sbin/create_c_stuff.pl";
require "lib/sbin/create_fortran_stuff.pl";
require "lib/sbin/GridFuncStuff.pl";

%thorns = &create_thorn_list;

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
  return ("flesh", "toolkits/test/flesh", 
	   "test1", "toolkits/test/test1", 
	   "test2", "toolkits/test/test2",
	   "test3", "toolkits/test/test3");
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

    
  

    
  
