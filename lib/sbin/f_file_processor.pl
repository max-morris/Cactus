#!/usr/bin/perl -sw
#/*@@
#  @file      f_file_processor.pl
#  @date      Jan 22 1995
#  @author    Paul Walker
#  @desc
#  Postprocessor for Fortran files.
#
#  Reads STDIN, writes to STDOUT.
#
#  removes all comments
#  replaces && with newline and tab to col 7
#  replaces &! with newline at col 0
#  replaces \ at end of line with proper Fortran continuation lines
#      (depending on -free_format)
#  Breaks lines greater than 72 cols
#  Does this using multi-line matching!
#
#  If run with -free_format, chooses free-format
#  line splitting.
#
#  @enddesc
#  @version $Header$
#@@*/

$MULTILINE_MATCHING = 1;                         # Multi-line is on!

# Do we want line directives?
$line_directives = $line_directives eq 'yes';

# Pick the correct set of comments to remove.
if ($free_format)
{
  $standard_comments = "\\s*[!]";
}
else
{
  $standard_comments = "[cC!]";
}

# Loop over all lines.
$line = 1;
$file = $source_file_name;
$autoline = 1;
$autofile = "";
while (<>)
{
  # Get rid of final \n
  chomp;

  # Handle directives
  if (/^\#/)
  {
    if ($line_directives)
    {
      # Handle line directives
      if (/^\#\s*(\d+)\s*"([^"]*)"/)
      {
        $line = $1;
        $file = $2;
        if ($file eq '<stdin>')
        {
          $file = $source_file_name;
        }
      } else {
        ++$line;
      }
      next;
    }
    else
    {
      # Ignore directives
      next;
    }
  }

  # Get rid of any tabs
  s/\t/        /g;

  # Chop Fortran comments to 132 columns (they stay in code)
  # removing any quotes
  # (standard c C, or even ! comments)
  if (/^$standard_comments.*$/)
  {
    # Remove quotes
    s/['"]//g;
    if (/(.{132,132})/)
    {
      &printline ($1);
    }
    else
    {
      &printline ($_);
    }
  }
  else
  {
    # Get rid of ! comments : a bit tricky as ! may appear inside strings

    # the following code by Fokke Dijkstra also checks for comments
    # on a line with a string
    # Search for possible comment
    if (/!/)
    {
      # find all ! " and ' and check for strings or comments
      $string = 0;
      while (m/([!"'])/g)
      {
        # keep track of position for substr include last character
        $position = (pos) - 1;

        # check if we are currently in a string and possibly end it,
        # or check for a new string or a comment
        if ($string)
        {
          if ($1 eq "\'" && $string == 1)
          {
            $string = 0;
          }
          if ($1 eq "\"" && $string == 2)
          {
            $string = 0;
          }
        }
        elsif ($1 eq "\'")
        {
          $string = 1;
        }
        elsif ($1 eq "\"")
        {
          $string = 2;
        }
        elsif ($1 eq "!")
        {
          $_ = substr ($_, 0, $position);
          last;
        }
      }
    }

    # Get rid of trailing blanks
    s/\s*$//;

    # Put in the line breaks (&&)
    if($free_format)
    {
      s/\s*\&\&\s*/\n  /g;
    }
    else
    {
      s/\s*\&\&\s*/\n      /g;
    }

    foreach my $LINE (split('\n',$_))
    {
      &splitline($LINE);
    }
  }

  ++$line;
}

#/*@@
#  @routine    splitline
#  @date       Wed Nov 24 12:14:55 1999
#  @author     Tom Goodale
#  @desc
#  Chooses the correct routine to split lines.
#  @enddesc
#@@*/
sub splitline
{
  my ($LINE) = @_;

  if($free_format)
  {
    &free_format_splitline($LINE);
  }
  else
  {
    &fixed_format_splitline($LINE);
  }

}

#/*@@
#  @routine    fixed_format_splitline
#  @date       1995
#  @author     Paul Walker
#  @desc
#  Splits lines for F77 or fixed-format F90
#  @enddesc
#@@*/
sub fixed_format_splitline
{
  my ($LINE) = @_;

  # Note the new treatement of comments with \S
  if ($LINE =~ /^([^\S].{71,71})/)
  {
    &printline ($1);
    $LINE =~ s/.{72,72}//;
    while ($LINE =~ /^(.{66,66})/)
    {
      &printline ("     &$1");
      $LINE =~ s/.{66,66}//;
    }
    &printline ("     &$LINE");
  }
  else
  {
    &printline ($LINE);
  }

}

#/*@@
#  @routine    free_format_splitline
#  @date       Thu Sep 30 12:05:36 1999
#  @author     Erik Schnetter
#  @desc
#  Splits lines for free-format Fortran 90.
#  @enddesc
#@@*/
sub free_format_splitline
{
  my ($LINE) = @_;
  my $OUT;

  if ($LINE =~ /^(.{78,78})../)
  {
    $OUT = $1;
    # Check if the line already has a continuation mark.
    $OUT = "$OUT&" if (! ($OUT =~ /\&\s*$/));
    &printline ($OUT);
    $LINE =~ s/.{78,78}//;

    while ($LINE =~ /^(.{76,76})/)
    {
      $LINE =~ /^(.{75,75})/;
      $OUT = $1;
      $OUT = "  &$OUT" if (! ($OUT =~ /^\s*\&/));
      $OUT = "$OUT&" if (! ($OUT =~ /\&\s*$/));
      &printline ($OUT);
      $LINE =~ s/.{75,75}//;
    }
    if ($LINE =~ /^\s*\&\s*$/)
    {
      &printline ("  &$LINE");
    }
    else
    {
      $OUT = $LINE;
      $OUT = "  &$OUT" if (! ($LINE =~ /^\s*\&/));
      &printline ($OUT);
    }
  }
  else
  {
    &printline ($LINE);
  }
}



# Print a line and append a newline
# Emit line number and file name directives if necessary
sub printline
{
  my ($LINE) = @_;
  
  if ($LINE eq '') {
    # don't print empty lines
  } else {
    if ($line_directives) {
      if ($file ne $autofile) {
        print "# $line \"$file\"\n";
        $autoline = $line;
        $autofile = $file;
      } elsif ($line ne $autoline) {
        if ($line>$autoline && $line<=$autoline+3) {
          while ($autoline!=$line) {
            print "\n";
            ++$autoline;
          }
        } else {
          print "# $line\n";
          $autoline = $line;
        }
      }
    }
    print "$LINE\n";
    ++$autoline;
  }
}
