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

$toolkit_dir = shift(@ARGV);

chdir $toolkit_dir;

open(TOOLKITS, "ls|");

while(<TOOLKITS>)
{
  chop;
  next if (m:^CVS$:);

  if( -d $_)
  {
    push (@toolkits, $_);
  }
}

close TOOLKITS;

foreach $toolkit (@toolkits)
{
  chdir $toolkit;

  open(THORNLIST, "ls|");
  
  while(<THORNLIST>)
  {
    chop;
    if( -d $_)
    {
      push(@total_thornlist, "$toolkit/$_");
    }
  }
  chdir "..";
}

foreach $thorn (@total_thornlist)
{
  print "$thorn\n";
}

