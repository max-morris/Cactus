#!/bin/perl -s
#
# Test Suite tool
# Version: $Header$

require "lib/sbin/RunTestUtils.pl";

# Read options from command line
$prompt = shift;   
$prompt =~ tr/A-Z/a-z/;
$home_dir = shift;
$config = shift;;

&PrintHeader;

# Set up RunTest configuration
%config_data = &Configure($config,$home_dir);

# Get the executable
$executable = ParseExecutable(%config_data);

# Look to see if MPI is dfined
$mpi = ParseExtras(%config_data);
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
%testdata = &FindAllTests(%config_data);

# Parse test parameter files
%testdata = &ParseAllParameterFiles(%testdata);

# Print database
#&PrintDataBase(%testdata);

while ($choice !~ /^Q/i)
{
  undef($thorn);
  undef($test);
  undef($choice);
  
  while (!($choice =~ /^Q/i) )
  {
      
    print "  --- Menu ---\n\n";
    
    print "  Run entire set of tests [E]\n";
    print "  Choose test from [T]horn or [A]rrangement\n";
    print "  Rerun previous test [R]\n";
    print "  Compare all files in the test output directories [O]\n";
    print "  Customize testsuite checking [C]\n";
    print "  Quit [Q]\n\n";
    $choice = &defprompt("  Select choice: ","E");
    print "\n";
    
    if ($choice =~ /^[EO]/i) 
    {

      # Reset/Initialise Test Statistics
      %testdata = &ResetTestStatistics(%testdata);

      # Run all parameter files
      foreach $thorn (split(" ",$testdata{"RUNNABLETHORNS"}))
      {
	foreach $test (split(" ",$testdata{"$thorn RUNNABLE"}))
	{
	  print "  Test $thorn: $test \n";
	  print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	  if ($choice !~ /^O/i)
	  {
	    %testdata = &RunTest($test,$thorn,%testdata);
	  }
	  %testdata = &CompareTestFiles($test,$thorn,%testdata);
	  %testdata = &ReportOnTest($test,$thorn,%testdata);
	}
      }

      # Write results of all tests
      &WriteFullResults(%testdata);
    } 
    elsif ($choice =~ /^[AT]/i)
    {
      ($ntests,@tests) = &ChooseTest($choice,%testdata);
      for ($i=0;$i<$ntests;$i++)
      {
	$test  = $tests[2*$i];
	$thorn = $tests[2*$i+1];
	print "  Test $thorn: $test\n";
	print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	%testdata = &RunTest($tests[2*$i],$tests[2*$i+1],%testdata);
	%testdata = &CompareTestFiles($tests[2*$i],$tests[2*$i+1],%testdata);
	%testdata = &ReportOnTest($tests[2*$i],$tests[2*$i+1],%testdata);
      }
    }
    elsif ($choice =~ /^R/i)
    {
      if ($thorn && $test)
      {
	print "  Test $thorn: $test \n";
	print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	%testdata = &RunTest($test,$thorn,%testdata);
	%testdata = &CompareTestFiles($test,$thorn,%testdata);
	%testdata = &ReportOnTest($test,$thorn,%testdata);
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
    else
    {
      print "  Choice not recognized, try again!\n";
    }
  }
  print "\n";
}

