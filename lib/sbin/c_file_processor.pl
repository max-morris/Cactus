#!/bin/perl

$home = shift(@ARGV);

if (! -e "$home/fortran_name.pl" ) 
{
    die "Unable to get fortran name file!";
}

require "$home/fortran_name.pl";

$/ = ";";
$*=1;


while(<>)
{
    $line = $_;
    if($line =~ m:FORTRAN_NAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

	@args = split(",", $arglist );

	$new = &fortran_name($args[$#args]);

	$line =~ s:FORTRAN_NAME\s*\(([^\)]*)\):$new:;
    }
    print $line;
}
