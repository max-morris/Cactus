#!/bin/perl
#/*@@
#  @file      c_file_processor.pl
#  @date      Fri Jan 22 18:09:47 1999
#  @author    Tom Goodale / Gerd Lanfermann / Thomas Radke
#  @desc
#             Processes certain things within a C source file
#             which can't be dealt with by the normal C preprocessor.
#
#             This script puts everything after a DECLARE_CCTK macro
#             until the end of the routine into a new block.
#             It also fixes the function names for fortran wrappers.
#  @enddesc
#  @version   $Header$
#@@*/

$home = shift(@ARGV);

# Do we want line directives?
$line_directives = $line_directives eq 'yes';

$fortran_name_file = "$home/fortran_name.pl";

if (! -e "$fortran_name_file" )
{
  die "Unable to get fortran name file $fortran_name_file!";
}

require "$fortran_name_file";

if($source_file_name)
{
  print "# 1 \"$source_file_name\"\n" if ($line_directives);
}

$closing_brackets  = '';
$routine  = '';
$n_arg_braces = -3;

# parse the file up to a ";\n"
$/ = ";\n";
$* = 1;

while (<>)
{
  # split in lines... and collect in routine;
  foreach $mline (split ("\n"))
  {
    # skip one-line comments
    # (note that this is still incomplete for multi-line C comments -
    #  it is not checked if some code follows after the closing '*/')
    if ($mline !~ m/^\s*\/\// && $mline !~ m/^\s*\/\*.*\*\/\s*$/)
    {
      # check if the DECLARE macros are found on a line
      if ($mline =~ s/(DECLARE_CCTK_(PARAMETERS|ARGUMENTS))(\s*;)?/$1 {/g)
      {
        $closing_brackets = "} /* closing bracket for $1 block */ " . $closing_brackets;
        $n_arg_braces = -1;
      }

      # Remove a ; from after the fileversion macro
      # such a semicolon could lead to warning messages.
      $mline =~ s/^\s*(CCTK_FILEVERSION\s*\([^)]*\))(\s*;)?/$1/;
      $mline =~ s/^\s*((ONE|TWO|THREE|FOUR|FIVE)_FORTSTRING_(CREATE|PTR)\s*\([^)]*\))(\s*;)?/$1/;

      # start counting braces if there has been a DECLARE macro
      if ($closing_brackets)
      {
        $n_arg_braces-- while ($mline =~ m/(})/g);
        $n_arg_braces++ while ($mline =~ m/({)/g);
      }

      $mline = "$closing_brackets$mline" if ($n_arg_braces == -1);
    }

    $routine .= $mline . "\n";

    if ($n_arg_braces == -1)
    {
      $closing_brackets = '';
      $n_arg_braces = -2;

      # call the fortran namefix routine/reset routine
      fixfnames ($routine);
      $routine = '';
    }
  }
}

fixfnames ($routine);


sub fixfnames
{
  my $myroutine = shift (@_);
  @flines = split /(;)/,$myroutine;

#  print $myroutine;

  foreach $fline (@flines)
  {
    while ($fline =~ m:CCTK_FNAME\s*\(([^\)]*)\):)
    {
      $arglist = $1;
      $arglist =~ s:[\s\n\t]+::g;

      @args = split(",", $arglist );

      $new = &fortran_name($args[$#args]);

      $fline =~ s:CCTK_FNAME\s*\(([^\)]*)\):$new:;
    }
    while ($fline =~ m:CCTK_FORTRAN_COMMON_NAME\s*\(([^\)]*)\):)
    {
      $arglist = $1;
      $arglist =~ s:[\s\n\t]+::g;

      @args = split(",", $arglist );

      $new = &fortran_common_name($args[$#args]);

      $fline =~ s:CCTK_FORTRAN_COMMON_NAME\s*\(([^\)]*)\):$new:;
    }

    print $fline;
  }
}
