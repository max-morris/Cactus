#/*@@
#  @file      CVSUpdate.pl
#  @date      Tue Nov 21 2000
#  @author    Gabrielle Allen
#  @desc 
#     Updates Cactus checkout
#     (avoids problems with different versions of cvs client)
#  @enddesc 
#  @version $Header$
#@@*/

$cvs_ops="-z6 -q";
$cvs_update_ops="-d -P";

require "lib/sbin/MakeUtils.pl";

$debug = 0;
if ($debug)
{
  print "DEBUG mode: cvs commands not issued\n\n";
}

print("\nUpdating Flesh\n");
$command = "cvs $cvs_ops update $cvs_update_ops CONTRIBUTORS COPYRIGHT Makefile lib doc src";
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
if (!$debug)
{
  open (CS, "$command |");
  while (<CS>) 
  {  
    print ;
  }
  close (CS);
}

($package_dir, $thornlist) = @ARGV;

if ($thornlist =~ /^$/) {
   %info = &buildthorns($package_dir,"thorns");
} else {
   %info = &ReadThornlist($thornlist);
}

$current_dir = `pwd`;
chdir $package_dir;
foreach $thorn (sort keys %info)
{

  if( ! -d "$thorn/CVS")
  {
    print "Ignoring $thorn - no CVS directory\n";
    next;
  }
  chdir $thorn;
  print("\nUpdating $thorn\n");
  $command = "cvs $cvs_ops update $cvs_update_ops";
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
  if (!$debug)
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
