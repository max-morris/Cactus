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


@parameter_structure = &create_parameter_structure(%parameter_database);


foreach $line (@parameter_structure)
{
  print "$line\n";
}

@parameter_declarations = &create_parameter_declarations(%parameter_database);


#&print_parameter_database(%parameter_database);

#&print_interface_database(%interface_database);

sub create_parameter_declarations
{
  local(@paramater_database) = @_;

  foreach $entry (@parameter_database)
  {
  }

}

sub create_parameter_structure
{
  local(%parameter_database) = @_;
  local(@structure);
  local($line, $entry, $thorn, $parameter, $type);

  $line = "struct CCTK_PARAMS {";

  push(@structure, $line);

  foreach $entry ( keys %parameter_database )
  {
    if($entry =~ m:([^ ]*) ([^ ]*) type:)
    {
      $thorn = $1;
      $parameter = "\L$2\E";
      if($parameter_database{$entry} eq "KEYWORD" ||
	 $parameter_database{$entry} eq "STRING"  ||
	 $parameter_database{$entry} eq "SENTENCE")
      {
	$type = "char *";
      }
      elsif($parameter_database{$entry} eq "LOGICAL" ||
	    $parameter_database{$entry} eq "INTEGER")
      {
	$type = "int ";
      }
      elsif($parameter_database{$entry} eq "REAL")
      {
	$type = "Double ";
      }
      else
      {
	die("Unknown parameter type '$parameter_database{$entry}'");
      }

      $line = "  ". $type . $thorn . "_". $parameter .";";

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
