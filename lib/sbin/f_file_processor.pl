#!/usr/local/bin/perl
#
# Version: $Id$
#
# New BreakLines routine which is much cleaner
#
# Reads STDIN, writes to STDOUT.
#
# replaces && with newline and tab to col 7
# replaces &! with newline at col 0
# Breaks lines greater than 72 cols
# Does this using multi-line matching!
#
# Paul, Jan 22 1995
# Joan, Apr 21 1997: get rid of cC comments and handle ! comments properly
#                    and fix it so now it is really 72 and we do not get
#                    shitty breaks in the middle of fortran strings!
#
$* = 1;				# Multi-line is on!

while (<>) {
    next if (/^\s*$/);		# Blanks slow down compilation, and cpp makes
				# lots and lots of them!

    next if (/^\#/);            # Remove any remaining # directives (e.g. line directives).


    # Get rid of final \n
    chop;

    # Get rid of any tabs
    s/\t/        /g;
    
    # OK, now put in the line breaks:
    s/\&\&\s*/\n      /g;
    s/\&\!\s*/\n/g;

    # Get rid of standard c C comments
    s/^[cC].*$/\n/g;
    
    # Get rid of ! comments : a bit tricky as ! may appear inside strings
    s/(.)![^'"]*$/\1\n/g;

    s/\s*\;\s*$//;

    # And now we can fix the lines.  This is actually a little complicated.
    # since there is a different case if the thing matches a newline
    # than if it doesn't.
    if (/\n/) {
	foreach $LINE (split('\n',$_)) {
	    &splitline($LINE);
	}
    } else {
	&splitline($_);
    }
}

sub
splitline 
{
    local ($LINE) = @_;
    # Remove ,, and , \) from blank thorns
    while ($LINE =~ s/,\s*,/,/) {}
    $LINE =~ s/,\s*\)/\)/;

    # Strip out leading spaces in favor of 7 spaces
    # $LINE =~ s/^\s+/       /;
    # Note the new treatement of comments with \S
    if ($LINE =~ /^([^\S].{71,71}).*/) {
        print "$1\n";
        $LINE =~ s/.{72,72}//;
        while ($LINE =~ /^(.{66,66}).*/) {
            print "     &$1\n";
            $LINE =~ s/.{66,66}//;
        }
        print "     &$LINE\n";
    } else {
        print "$LINE\n";
    }

}
