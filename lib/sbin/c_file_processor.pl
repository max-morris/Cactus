#!/bin/perl
#/*@@
#  @file      c_file_processor.pl
#  @date      Fri Jan 22 18:09:47 1999
#  @author    Tom Goodale
#  @desc 
#  Processes a c file replacing certain strings which can't be dealt
#  with by the normal c preprocessor.
#  @enddesc 
#@@*/

$home = shift(@ARGV);

$fortran_name_file = "$home/fortran_name.pl";

if (! -e "$fortran_name_file" ) 
{
    die "Unable to get fortran name file $fortran_name_file!";
}

require "$fortran_name_file";

$/ = ";";
$*=1;


while(<>)
{
    $line = $_;
    while($line =~ m:FORTRAN_NAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

	@args = split(",", $arglist );

	$new = &fortran_name($args[$#args]);

	$line =~ s:FORTRAN_NAME\s*\(([^\)]*)\):$new:;
    }
    while($line =~ m:FORTRAN_COMMON_NAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

	@args = split(",", $arglist );

	$new = &fortran_common_name($args[$#args]);

	$line =~ s:FORTRAN_COMMON_NAME\s*\(([^\)]*)\):$new:;
    }

    print $line;
}
