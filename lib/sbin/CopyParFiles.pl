#!/bin/perl -s
#
# Script to copy parameter files
# Version: $Id$

$home = `pwd`;
chop($home);

$config = $ARGV[0];

open(THORNLIST,"<configs/$config/ThornList") || die "ThornList not available";

if (! -d "thornparfiles")
{
    mkdir("thornparfiles", 0755) || die "Unable to create thornparfiles directory";
}

while (<THORNLIST>)
{
 /^(\S*)/;
 next if (m:^\#:);
 $thorn=$1;
 print "Thorn $thorn\n";
 $thorn = "arrangements/$thorn/par";
 if (-d $thorn)
 {
   chdir $thorn;
   while ($parfile = <*.par>)
   { 
     if (-e "$home/thornparfiles/$parfile")
     {
        print "  $parfile exists, will not overwrite\n";
     }
     else
     {
       print "  Copying $parfile\n";
       system("cp $parfile $home/thornparfiles/$parfile");
     }
   }
   
   chdir "$home${sep}";

 }
}
