#! /usr/bin/perl
#/*@@
#  @file      BuildActiveThorns.pl
#  @date      Tue Jan 19 14:02:07 1999
#  @author    Tom Goodale
#  @desc 
#  Creates an ActiveThornsList
#  @enddesc 
#  @version $Id$
#@@*/

$package_dir = shift(@ARGV);

chdir $package_dir;

open(PACKAGES, "ls|");

while(<PACKAGES>)
{
  chop;

  # Ignore CVS and backup stuff
  next if (m:^CVS$:);
  next if (m:^\#:);
  next if (m:~$:);

  # Just pick directories
  if( -d $_)
  {
    push (@packages, $_);
  }
}

close PACKAGES;

foreach $package (@packages)
{
  chdir $package;

  open(THORNLIST, "ls|");
  
  while(<THORNLIST>)
  {
    chop;

    # Ignore CVS and backup stuff
    next if (m:^CVS$:);
    next if (m:^\#:);
    next if (m:~$:);
    
    # Allow each package to have a documentation directory.
    next if (m:^doc$:);

    # Just pick directories
    if( -d $_)
    {
      push(@total_thornlist, "$package/$_");
    }
  }
  chdir "..";
}

foreach $thorn (@total_thornlist)
{
  print "$thorn\n";
}

