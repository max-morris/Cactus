#!/bin/perl -s
#
# Test Suite tool
# Version: $Header$

require "lib/sbin/RunTestUtils.pl";

$prompt = shift;
$home_dir = shift;

$prompt =~ tr/A-Z/a-z/;

$config = shift;;

# Set up RunTest configuration
%config_data = &Configure($config,$home_dir);
$sep= "/";

&PrintHeader;

# Look to see if MPI is dfined
$mpi = ParseExtras(%config_data);

# Get the executable
$executable = ParseExecutable(%config_data);

if ($mpi)
{
  $numprocs = &defprompt("  Enter number of processors","2");
  $command = &defprompt("  Enter command to run executable","mpirun -np $numprocs ");
}
else
{
  $command = &defprompt("  Enter command to run executable"," ");
}

# Initialise testdata database
%testdata = &InitialiseTestData();

# Find test parameter files
%testdata = &ParseTests(%config_data);

# Parse test parameter files
%testdata = &ParseAllParameterFiles(%testdata);

# Reset/Initialise Test Statistics
%testdata = &ResetTestStatistics(%testdata);

# Print database
#&PrintDataBase(%testdata);
$loop = 1;
$run = 1;

$tests = &defprompt("  Run All tests or go to Menu","All");

while ($loop == 1)
{
  if ($tests =~ /^A/i) 
  {
    # Run all parameter files
    foreach $thorn (split(" ",$testdata{"RUNNABLETHORNS"}))
    {
      foreach $test (split(" ",$testdata{"$thorn RUNNABLE"}))
      {
	print "  Test $thorn: $test \n";
	print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	if ($run)
	{
	  %testdata = &RunTest($test,$thorn,%testdata);
	}
	%testdata = &CompareTestFiles($test,$thorn,%testdata);
      }
    }

    # Write results of all tests
    &WriteFullResults(%testdata);

    $loop = 0;
  } 
  elsif ($tests =~ /^M/i)
  {
    undef($thorn);
    undef($test);
    undef($choice);

    while (!($choice =~ /^[EOQ]/i) )
    {

      print "\n  --- Menu ---\n\n";
      
      print "  Choose test from [T]horn or [A]rrangement\n";
      print "  Rerun previous test [R]\n";
      print "  Run entire set of tests [E]\n";
      print "  Compare all files in the test output directories [O]\n";
      print "  Customize testsuite checking [C]\n";
      print "  Quit [Q]\n\n";
      $choice = &defprompt("  Select choice: ","T");

      if ($choice =~ /^[ATC]/i)
      {
	($test,$thorn) = &ChooseTest($choice,%testdata);
	%testdata = &RunTest($test,$thorn,%testdata);
	%testdata = &CompareTestFiles($test,$thorn,%testdata);
      }
      elsif ($choice =~ /^R/i)
      {
	if ($thorn && $test)
	{
	  print "  Running $thorn: $test \n";
	  print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	  %testdata = &RunTest($test,$thorn,%testdata);
	  %testdata = &CompareTestFiles($test,$thorn,%testdata);
	}
	else
	{
	  print "  No previous test has been run\n";
	}
      }
      elsif ($choice =~ /^C/i)
      {
	print "  Options for customization\n";
	if ($test)
	{
	  print "    Change tolerance for this run ($test) [R]\n";
	}
	print "    Change tolerance from $testdata{\"TOLERANCE\"} for all further runs [T]\n";
	$choice = &defprompt("  Select choice: ","");
	if ($choice =~ /T/i)
	{
	  $testdata{"TOLERANCE"} = &defprompt("  New tolerance: ","$testdata{\"TOLERANCE\"}");
	}
      }
      elsif ($choice =~ /^Q/i)
      {
	$loop = 0;
      }
      elsif ($choice =~ /^E/i)
      {
	$tests = "A";
      }
      elsif ($choice =~ /^O/i)
      {
	$tests = "A";
	$run = 0;
      }
      else
      {
	print "  Choice not recognized, try again!\n";
      }
    }
  }
  elsif ($tests =~ /^Q/i)
  {
    $loop = 0;
  }
  else
  {
    print "  Choice not recognized, try again!\n";
  }

  print "\n";
}

