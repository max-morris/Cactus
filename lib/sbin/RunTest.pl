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

# Initialise testdata database
%testdata = &InitialiseTestData();

%runconfig = &InitialiseRunData();

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
      
    print "------------------------------------------------------------------------\n\n";
    print "  --- Menu ---\n\n";
    
    print "  Run entire set of tests [E]\n";
    print "  Run entire set of tests interactively [I]\n";
    print "  Choose test from [T]horn or [A]rrangement\n";
    print "  Rerun previous test [R]\n";
    print "  Compare all files in the test output directories [O]\n";
    print "  Customize testsuite checking [C]\n";
    print "  Quit [Q]\n\n";
    $choice = &defprompt("  Select choice: ","E");
    print "\n";
    
    if ($choice =~ /^[EIO]/i) 
    {

      # Reset/Initialise Test Statistics
      \%rundata = &ResetTestStatistics(\%rundata,%testdata);

      # Run all parameter files
      foreach $thorn (split(" ",$testdata{"RUNNABLETHORNS"}))
      {
	foreach $test (split(" ",$testdata{"$thorn RUNNABLE"}))
	{
	  print "------------------------------------------------------------------------\n\n";
	  print "  Test $thorn: $test \n";
	  print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	  if ($choice !~ /^O/i)
	  {
	    %testdata = &RunTest($test,$thorn,\%config_data,%testdata);
	  }
	  %rundata = &CompareTestFiles($test,$thorn,\%runconfig,\%rundata,\%config_data,%testdata);
	  %rundata = &ReportOnTest($test,$thorn,\%rundata,%testdata);
	  if ($choice =~ /^I/i)
	  {
	    &ViewResults($test,$thorn,\%runconfig,\%rundata,%testdata);
	  }
	}
      }

      # Write results of all tests
      &WriteFullResults(\%rundata,%testdata);
    } 
    elsif ($choice =~ /^[AT]/i)
    {
      ($ntests,@tests) = &ChooseTests($choice,%testdata);
      for ($i=0;$i<$ntests;$i++)
      {
	$test  = $tests[2*$i];
	$thorn = $tests[2*$i+1];
	print "------------------------------------------------------------------------\n\n";
	print "  Test $thorn: $test\n";
	print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	%testdata = &RunTest($tests[2*$i],$tests[2*$i+1],\%config_data,%testdata);
	%rundata = &CompareTestFiles($tests[2*$i],$tests[2*$i+1],\%runconfig,\%rundata,\%config_data,%testdata);
	%rundata = &ReportOnTest($tests[2*$i],$tests[2*$i+1],\%rundata,%testdata);
	&ViewResults($tests[2*$i],$tests[2*$i+1],\%runconfig,\%rundata,%testdata);
      }
    }
    elsif ($choice =~ /^R/i)
    {
      if ($thorn && $test)
      {
	print "------------------------------------------------------------------------\n\n";
	print "  Test $thorn: $test \n";
	print "    \"$testdata{\"$thorn $test DESC\"}\"\n";
	%testdata = &RunTest($test,$thorn,\%config_data,%testdata);
	%rundata = &CompareTestFiles($test,$thorn,\%runconfig,\%rundata,\%config_data,%testdata);
	%rundata = &ReportOnTest($test,$thorn,,\%rundata,%testdata);
	&ViewResults($test,$thorn,\%runconfig,\%rundata,%testdata);
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
      print "    Change tolerance from $rundata->{\"TOLERANCE\"} for all further runs [T]\n";
      $choice = &defprompt("  Select choice: ","");
      if ($choice =~ /T/i)
      {
	$rundata->{"TOLERANCE"} = &defprompt("  New tolerance: ","$rundata->{\"TOLERANCE\"}");
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

