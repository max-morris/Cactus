#!/usr/bin/perl -w
#/*@@
#  @routine    FixPageNumbersInPostscript.pl
#  @date       Fri 15 Mar 2002
#  @author     Thomas Radke
#  @desc
#              Fixes the line numbers in the postscript versions of the
#              Cactus UsersGuide, ThornGuide, and MaintGuide
#  @enddesc
#  @version    $Header$
#@@*/

# $part counts the parts (chapters) in the postscript file
$part = 0;

# @part_letters lists the numbering for all the parts
# the first two parts (title page and table of contents) are skipped
@part_letters = ('', '', 'A' .. 'Z');

# $last_line must match EOP in order to check for a new page
$last_line = "eop\n";


# skip all lines in the postscript setup and prolog
while (<>)
{
  print;
  last if (/^%%EndProlog\n$/);
}


while (<>)
{
  if ($last_line =~ /eop$/ && /^%%Page: (\d+) (\d+)$/)
  {
    $part++ if ($1 == 1);
    $_ = "%%Page: ${part_letters[$part]}$1 $2\n"
      if defined(${part_letters[$part]});
  }

  print;
  $last_line = $_;
}

1;
