#! /usr/bin/perl -s
#/*@@
#  @file      configure.pl
#  @date      Fri Jan  8 15:06:22 1999
#  @author    Tom Goodale
#  @desc 
#  Prototype configure script for the CCTK
#  @enddesc 
#@@*/

$tmphome = shift(@ARGV);

if(! $compiler)
{
  $compiler="f90"
}

print "Determining number of fortran underscores...\n";

# Create a test file
open(OUT, ">fname_test.f") || die "Cannot open fname_test.f\n";

print OUT <<EOT;
      subroutine test_name(a)
      integer a
      a = 1
      return
      end

EOT

close OUT;

# Compile the test file
print "Compiling test file with $compiler...\n";
system("$compiler -c fname_test.f");

$retcode = $? >> 8;

if($retcode > 0)
{
    print "Failed to compile fname_test.f\n";
}


# Search the object file for the appropriate symbols
open(IN, "<fname_test.o") || die "Cannot open fname_test.o\n";

while(<IN>)
{
    $line = $_;
    if($line =~ m:(TEST_NAME)(_*):i)
    {
	$name = $1;
	$underscores = $2;

	if($name =~ m:TEST_NAME:)
	{
	    print "Uppercase - ";
	    $case = 1;
	}
	if($name =~ m:test_name:)
	{
	    print "Lowercase - ";
	    $case = 0;
	}
	if($underscores eq "")
	{
	    print " No trailing underscore\n";
	    $n_underscores = 0;
	}
	if($underscores eq "_")
	{
	    print "One trailing underscore\n";
	    $n_underscores = 1;
	}
	if($underscores eq "__")
	{
	    print "Two trailing underscores\n";
	    $n_underscores = 2;
	}
    }
    
}

close IN;

# Delete the temporary files
unlink <fname_test.*>;

# Determine the case and number of underscores
if($n_underscores == 0)
{
    $normal_suffix = "";
    $underscore_suffix = "";
}

if($n_underscores == 1)
{
    $normal_suffix = "_";
    $underscore_suffix = "_";
}

if($n_underscores == 2)
{
    $normal_suffix = "_";
    $underscore_suffix = "__";
}

if($case = 0)
{
    $case_prefix = "\\L";
}
if($case = 1)
{
    $case_prefix = "\\L";
}


# Create the perl module to map the fortran names.
open(OUT, ">$tmphome/fortran_name.pl") || die "Cannot create fortran_name.pl\n";
print OUT <<EOT;
#!/bin/perl

sub fortran_name
{
    local(\$old_name) = \@_;
    local(\$new_name);

    \$new_name = "$case_prefix\$old_name\\E";

    if(\$new_name =~ m:_: ) 
    {
	\$new_name = \$new_name."$underscore_suffix";
    }
    else
    {
	\$new_name = \$new_name."$normal_suffix";
    }

    return \$new_name;
}

1;
    
EOT

close OUT;



