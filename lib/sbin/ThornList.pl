#!/usr/bin/perl

#/*@@
#  @file      ThornList.pl 
#  @date      March 2001
#  @author    Ian Kelley
#  @desc 
#      Either creates a stripped down thornlist of all thorns in the arrangments
#      directory, or prints out the full paths to each thorn on a single line
#
#      Used primary by the ThornGuide Makefile
#  @enddesc 
#  @version 
#@@*/

# give help
if ($h || $help) {
   print <<EOC;
--> ThornList.pl <--

Either creates a stripped down thornlist of all thorns in the arrangments directory, or prints out the full paths to each thorn on a single line

Usage:
\t\$ perl -s ThornList.pl -arrangements_dir=/home/ikelley/Cactus/WaveToyC_Dev/arrangements > allthorns.th
\t\$ perl -s ThornList.pl -arrangements_dir=/home/ikelley/Cactus/WaveToyC_Dev/arrangements -thornlist=allthorns.th
EOC

exit 0;
}

if (! defined $arrangements_dir) {
   die "\nArrangements directory not defined (-arrangments_dir)";
} elsif ($arrangments_dir !~ /\/$/) {
   $arrangements_dir .= '/';
}

# if we are building a thornlist from thorns in $arrangements_dir
if (! defined $thornlist) 
{
   @arrangements = &FindDirectories($arrangements_dir);

   foreach $arrangement (@arrangements) 
   {
      @thorns = &FindDirectories("$arrangements_dir$arrangement");

      foreach $thorn (@thorns) {
         print "\n$arrangement/$thorn";
      }
   }
} 
# if we are printing all the thorn directories on one line
# for use by the ThornGuide makefile
else 
{
   $thorn_paths = "";

   open (THORNLIST, "<$thornlist") || die "\nCannot open thornlist ($thornlist): $!";

   while (<THORNLIST>) 
   {
      next if /\s*?\!/;          # bypass any directives
      s/(.*?)#.*/\1/;            # read up to the first "#"
      s/\s+//g;                  # replace any spaces with nothing
      
      next if ! /\w/;            # nothing on this line?

      $thorn_paths .= "$arrangements_dir$_ ";

   }

   close (THORNLIST);

   print $thorn_paths;
}
## END: MAIN ##

#/*@@
#  @routine   FindDirectories
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#  Grabs all directories within a given directory, minus the ones created by CVS
#  @enddesc 
#  @version 
#@@*/

sub FindDirectories 
{
   my (@good_directories);

   chdir ("$start_directory") || die "\nCannot chdir to $start_directory: $!";

   chdir("$_[0]") || die "\nCannot change directory to $_[0] : $!";

   open(LS, "ls -F|");

   while(chomp($name = <LS>)) 
   {
      next if (! -d $name);
      if (($name ne "History/") && ($name ne "CVS/"))  
      {
         $name =~ s#/##;
         push(@good_directories, $name);
      }
   }

   close(LS);

   chdir ($start_directory);
   return (@good_directories);
} ## END :Find_Directories:
