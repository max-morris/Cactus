#/*@@
#  @file      CVSUpdate.pl
#  @date      Tue Nov 21 2000
#  @author    Gabrielle Allen
#  @desc 
#     Updates Cactus checkout
#     (avoids problems with different versions of cvs client)
#     $Header$
#  @enddesc 
#@@*/

require "lib/sbin/MakeUtils.pl";

$network = 1;
if (!$network)
{
  print "DEBUG mode: cvs commands not issued\n\n";
}
$debug = 0;

print("\nUpdating Flesh\n");
$command = "cvs -z 9 -q update -d CONTRIBUTORS COPYRIGHT Makefile lib doc src";
if ($debug)
{
  $this_dir = `pwd`;
  chop($this_dir);
  print "\nIn directory $this_dir\n";
  print "Issuing command\n  $command\n";
  foreach $file (`ls CVS`)
  {
    chop($file);
    print "Contents of $file\n";
    open (FILE, "<CVS/$file") || die "Could not open CVS file";
    while (<FILE>)
    {
      print;
    }
  }
}
if ($network)
{
  open (CS, "$command |");
  while (<CS>) 
  {  
    print ;
  }
  close (CS);
}

$package_dir = shift(@ARGV);

%info = &buildthorns($package_dir,"thorns");

$current_dir = `pwd`;
chdir $package_dir;
foreach $thorn (sort keys %info)
{
  chdir $thorn;
  print("\nUpdating $thorn\n");
  $command = "cvs -z 9 -q update -d";
  if($debug)
  {
    $this_dir = `pwd`;
    chop($this_dir);
    print "In directory $this_dir\n";
    print "Issuing command\n  $command\n";
    foreach $file (`ls CVS`)
    {
      chop($file);
      print "Contents of $file\n";
      open (FILE, "<CVS/$file") || die "Could not open CVS file";
      while (<FILE>)
      {
	print;
      }
    }
  }
  if ($network)
  {
    open (CS, "$command |");
    while (<CS>) 
    {  
      print ;
    } 
  }
  chdir "../..";
}
chdir $current_dir;


exit;

