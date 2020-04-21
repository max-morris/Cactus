#!/bin/perl -s
#
# Test Suite tool

# For debugging:
my $debug = 0;
my $debug_indent_level = 0;
sub debug_print
{
  # debug statements are prefixed by a '#', to make them stand out from 
  # ordinary output
  $debug and print '#'.' 'x$debug_indent_level."@_\n";
}
sub debug
{
  $debug;
}
sub debug_indent
{
  $debug_indent_level++;
}
sub debug_dedent
{
  $debug_indent_level--;
}

use FindBin;
use lib "$FindBin::Bin";
require "RunTestUtils.pl";

# Read options from command line
$prompt = shift;   
$prompt =~ tr/A-Z/a-z/;
$home_dir = shift;
$config = shift;

&PrintHeader;

# Set up RunTest configuration
$config_data = &Configure($config,$home_dir,$prompt);

%runconfig = &InitialiseRunData();

# ----------------------------------------------------

# Sort out the static test data

# Initialise testdata database
$testdata = &InitialiseTestData();

# Find test parameter files
$testdata = &FindTestParameterFiles($testdata,$config_data);

# Parse test config files
$testdata = &ParseTestConfigs($testdata,$config_data,\%runconfig);

# Parse test parameter files
$testdata = &ParseAllParameterFiles($testdata,$config_data,\%runconfig);

# Print database
#&PrintDataBase($testdata);

# ----------------------------------------------------

$haverunall = 0;

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
    print "  Rerun previous test and show run output on screen [S]\n";
    print "  Print tolerance table for data in previous test [P]\n";
    print "  Compare all files in the test output directories [O]\n";
    print "  Customize testsuite checking [C]\n";
    print "  Quit [Q]\n\n";

    if ($haverunall == 1)
    {
      $choice = &defprompt("  Select choice: ","Q");
    }
    else
    {
      $choice = &defprompt("  Select choice: ","E");
    }

    print "\n";
    
    if ($choice =~ /^[EIO]/i) 
    {
      print "  Processes: $config_data->{'NPROCS'}\n\n";

      # Reset/Initialise Test Statistics
      $rundata = &ResetTestStatistics($rundata,$testdata);

      my $testno = 0;
      my $parallel_size = defined($ENV{CCTK_TEST_SIZE}) ? 0+$ENV{CCTK_TEST_SIZE} : 1;
      my $parallel_rank = defined($ENV{CCTK_TEST_RANK}) ? 0+$ENV{CCTK_TEST_RANK} : 1;

      # Run all parameter files
      foreach $thorn (sort split(" ",$testdata->{"RUNNABLETHORNS"}))
      {
#        foreach $test (sort split(" ",$testdata->{"$thorn RUNNABLE"}))
#        {
#          $testno++;
#          my $rank_to_run = ($testno % $parallel_size) + 1;
#          
#          print "------------------------------------------------------------------------\n\n";
#          #print "test=$test testno=$testno rank_to_run=$rank_to_run parallel_size=$parallel_size parallel_rank=$parallel_rank\n";

#          if ($rank_to_run != $parallel_rank) {
#            print "SKIPPING $test SEE RANK $rank_to_run\n";
#            next;
#          }

#          print "  Test $thorn: $test \n";
#          print "    \"$testdata->{\"$thorn $test DESC\"}\"\n";

#          if ($choice !~ /^O/i)
#          {
#            $testdata = &RunTest("log",$test,$thorn,$config_data,$testdata);
#          }

#          $rundata = &CompareTestFiles($test,$thorn,\%runconfig,$rundata,$config_data,$testdata);

#          $rundata = &ReportOnTest($test,$thorn,$rundata,$testdata);
#          if ($choice =~ /^I/i)
#          {
#            &ViewResults($test,$thorn,\%runconfig,$rundata,$testdata);
#          }
#        }
	foreach $test (split(" ",$testdata->{"$thorn RUNNABLE"}))
	{
	  print "------------------------------------------------------------------------\n\n";
	  print "  Test $thorn: $test \n";
	  print "    \"$testdata->{\"$thorn $test DESC\"}\"\n";
          my $retcode = 0;
	  if ($choice !~ /^O/i)
	  {
	    $retcode = &RunTest("log",$test,$thorn,$config_data,$testdata,\%runconfig);
	  }

	  $rundata = &CompareTestFiles($test,$thorn,\%runconfig,$rundata,$config_data,$testdata,$retcode);

	  $rundata = &ReportOnTest($test,$thorn,$rundata,$testdata);
	  if ($choice =~ /^I/i)
	  {
	    &ViewResults($test,$thorn,\%runconfig,$rundata,$testdata);
	  }
	}
      }

      # Write results of all tests
      &WriteFullResults($rundata,$testdata,$config_data);
      
      $haverunall = 1;

    } 
    elsif ($choice =~ /^[AT]/i)
    {
      ($ntests,@tests) = &ChooseTests($choice,$testdata);
      for ($i=0;$i<$ntests;$i++)
      {
#        $test  = $tests[2*$i];
#        $thorn = $tests[2*$i+1];
#        print "------------------------------------------------------------------------\n\n";
#        print "  Test $thorn: $test\n";
#        print "    \"$testdata->{\"$thorn $test DESC\"}\"\n";
#        $testdata = &RunTest("log",$tests[2*$i],$tests[2*$i+1],$config_data,$testdata);
#        $rundata = &CompareTestFiles($tests[2*$i],$tests[2*$i+1],\%runconfig,$rundata,$config_data,$testdata);
#        $rundata = &ReportOnTest($tests[2*$i],$tests[2*$i+1],$rundata,$testdata);
#        &ViewResults($tests[2*$i],$tests[2*$i+1],\%runconfig,$rundata,$testdata);
	$test  = $tests[2*$i];
	$thorn = $tests[2*$i+1];
	print "------------------------------------------------------------------------\n\n";
	print "  Test $thorn: $test\n";
	print "    \"$testdata->{\"$thorn $test DESC\"}\"\n";
        my $retcode = 0;
	$retcode = &RunTest("log",$tests[2*$i],$tests[2*$i+1],$config_data,$testdata,\%runconfig);
	$rundata = &CompareTestFiles($tests[2*$i],$tests[2*$i+1],\%runconfig,$rundata,$config_data,$testdata,$retcode);
	$rundata = &ReportOnTest($tests[2*$i],$tests[2*$i+1],$rundata,$testdata);
	&ViewResults($tests[2*$i],$tests[2*$i+1],\%runconfig,$rundata,$testdata);
      }
    }
    elsif ($choice =~ /^[RS]/i)
    {
      if ($thorn && $test)
      {
#        print "------------------------------------------------------------------------\n\n";
#        print "  Test $thorn: $test \n";
#        print "    \"$testdata->{\"$thorn $test DESC\"}\"\n";
#        if ($choice =~ /^S/i)
#        {
#          $testdata = &RunTest("log stdout",$test,$thorn,$config_data,$testdata);
#        }
#        else
#        {
#          $testdata = &RunTest("log",$test,$thorn,$config_data,$testdata);
#        }
#        $rundata = &CompareTestFiles($test,$thorn,\%runconfig,$rundata,$config_data,$testdata);
#        $rundata = &ReportOnTest($test,$thorn,,$rundata,$testdata);
#        &ViewResults($test,$thorn,\%runconfig,$rundata,$testdata);
	print "------------------------------------------------------------------------\n\n";
	print "  Test $thorn: $test \n";
	print "    \"$testdata->{\"$thorn $test DESC\"}\"\n";
        my $retcode = 0;
	if ($choice =~ /^S/i)
	{
	  $retcode = &RunTest("log stdout",$test,$thorn,$config_data,$testdata,\%runconfig);
	}
	else
	{
	  $retcode = &RunTest("log",$test,$thorn,$config_data,$testdata,\%runconfig);
	}
	$rundata = &CompareTestFiles($test,$thorn,\%runconfig,$rundata,$config_data,$testdata,$retcode);
	$rundata = &ReportOnTest($test,$thorn,,$rundata,$testdata);
	&ViewResults($test,$thorn,\%runconfig,$rundata,$testdata);
      }
      else
      {
        print "  No previous test has been run\n";
      }
    }
    elsif ($choice =~ /^P/i)
    {
      if ($thorn && $test)
      {
        &PrintToleranceTable($test,$thorn,$testdata,\%runconfig);
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
        while ($choice !~ /^[AR]/i)
        {
          print "    Change absolute tolerance for this run ($test) [A]\n";
          print "    Change relative tolerance for this run ($test) [R]\n";
          $choice = &defprompt("  Select choice: ","");
          if ($choice =~ /A/i)
          {
            $runconfig{"$thorn $test ABSTOL"}{".*"} = &defprompt("  New absolute tolerance: ","$runconfig{\"$thorn $test ABSTOL\"}{\".*\"}");
          }
          elsif ($choice =~ /R/i)
          {
            $runconfig{"$thorn $test RELTOL"}{".*"} = &defprompt("  New relative tolerance: ","$runconfig{\"$thorn $test RELTOL\"}{\".*\"}");
          }
        }
      }
      else
      {
        while ($choice !~ /^[AR]/i)
        {
          print "    Change absolute tolerance from $runconfig{\"ABSTOL\"} for all further runs [A]\n";
          print "    Change relative tolerance from $runconfig{\"RELTOL\"} for all further runs [R]\n";
          $choice = &defprompt("  Select choice: ","");
          if ($choice =~ /^A/i)
          {
            $runconfig{"ABSTOL"} = &defprompt("  New absolute tolerance: ","$runconfig{\"ABSTOL\"}");
          }
          elsif ($choice =~ /^R/i)
          {
            $runconfig{"RELTOL"} = &defprompt("  New relative tolerance: ","$runconfig{\"RELTOL\"}");
          }
        }
      }
    }
    elsif ($choice =~ /^Q/i)
    {
#      $loop = 0;
    }
    else
    {
      print "  Choice not recognized, try again!\n";
    }
  }
  print "\n";
}
