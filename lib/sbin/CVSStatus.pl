#/*@@
#  @file      CVSStatus.pl
#  @date      Mon Mar  6 01:20:01 CET 2000
#  @author    Gabrielle Allen
#  @desc 
#     Processes output from cvs status and gives better messages
#     Original version by Paul Walker
#     $Header$
#  @enddesc 
#@@*/

&writeheader();

open (CS, "cvs status 2>&1 |");
$gotone = 0;

while (<CS>) {
    if (m/waiting for/) {
	print;
    }
    if (m/Examining/) {
	if ($gotone) {
	  $dir =~ m:^\s*(/\w*/\w*):;
	  $dir = $1;
	  if ($dir ne $dir_old) {
	    print "\n$dir\n";$dir_old=$dir;
	  }
	  write;
	  # Get differences between versions
          if ($case =~ /diff/i)
	  {
	    print "\n\ncvs diff -r $rversion $thisdir/$file\n\n";
	    open (DIFF, "cvs diff -r $rversion $thisdir/$file |");
	    while (<DIFF>) {print;}
	  }
	}
	$gotone = 0;
	$thisdir="";
	$file = ""; 
	$dir = ""; 
	$status = ""; 
	$version=""; 
	$rversion="";
    }
    if (m/File: (\S+)/) {$file = $1;}
    if (m/Status: (.+)\s/) {
	$status = $1;
	if (!($status =~ m/Up-to-date/)) {
	    $gotone = 1;
	}
    }
    if (m/Examining\s*(\S+)\s*$/) {$thisdir = $1;}
    if (m/Working revision:\s*(\S+)\s/) {$version = $1;}
    if (m/Repository revision:\s*(\S+)\s+(\S+)/) {
	$rversion = $1; $dir = $2;
    }
    
}
exit;

format STDOUT =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< @<<<<<<<<<<<<<<< @<<<<<<</@<<<<<<<
$file, $status, $version, $rversion
.


sub writeheader
{
local ($case);
if ($case =~ "diff")
{
print <<EOF;

CVS Diff report 

File                                         Status          Local/Remote ver
-------------------------------------------------------------------------------
}
else
{
print <<EOF;

CVS Status report 

File                                         Status          Local/Remote ver
-------------------------------------------------------------------------------
EOF
}
}
