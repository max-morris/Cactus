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

open (CS, "cvs -z 9 -q update -d CONTRIBUTORS COPYRIGHT Makefile lib doc src|");
while (<CS>) {  
  print ;
}
close (CS);

$package_dir = shift(@ARGV);

%info = &buildthorns($package_dir,"thorns");

$current_dir = `pwd`;
chdir $package_dir;
foreach $thorn (sort keys %info)
{
  chdir $thorn;
  print("Updating $thorn\n");
  open (CS, "cvs -z 9 -q update -d |");
  while (<CS>) 
  {  
    print ;
  } 
  chdir "../..";
}
chdir $current_dir;


exit;

