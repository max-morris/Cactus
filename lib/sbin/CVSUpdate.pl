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

open (CS, "cvs -z 9 -q update CONTRIBUTORS COPYRIGHT Makefile lib doc src|");
while (<CS>) {  
  print ;
}
close (CS);

$package_dir = shift(@ARGV);

%info = &buildthorns($package_dir,"thorns");

print "dir is $package_dir\n";

foreach $thorn (sort keys %info)
{
  print("$thorn");
}


exit;

