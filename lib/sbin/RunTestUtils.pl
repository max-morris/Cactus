sub Configure
{
  my($config,$home_dir,$prompt) = @_;
  my($configs_dir,$tests_dir);

  # Cactus home directory
  $config_data->{"CCTK_DIR"} = $home_dir;

  # Interactive of not
  $config_data->{"PROMPT"} = $prompt;

  # Cactus configurations directory
  if($ENV{"CONFIGS_DIR"})
  {
    $configs_dir = $ENV{"CONFIGS_DIR"};
  } 
  else
  {
    $configs_dir = "$homedir/configs";
  }
  $config_data->{"CONFIGSDIR"} = $configs_dir;

  # Cactus test directory
  if ($ENV{"TESTS_DIR"})
  {
    $tests_dir = $ENV{"TESTS_DIR"};
  }
  else
  {
    $tests_dir = $home_dir."/TEST";
  }
  $config_data->{"TESTS_DIR"} = $tests_dir;

  $config_data->{"SEPARATOR"} = "/";
  $config_data->{"CONFIG"} = $config;

  # Get the executable
  $config_data = FindExecutionDetails($config_data);

  return $config_data;
}

sub MissingThorns
{
  my($active,$allthorns) = @_;
  my(@at,$th,$foundit,$thornpart,$nmissing,$missing);

  @at = split(' ',$active);
  $nmissing = 0;
  $missing = "";
  foreach $th (@at)
  {
    $th = "\U$th";
    $foundit = 0;
    
    foreach $tthorn  (split(" ",$allthorns))
    {
      $thornpart = "\U$tthorn";
      if ($thornpart eq $th)
      {
        $foundit = 1;
      }
    }      
    if (!$foundit)
    {
      $missing .=" $th";
      $nmissing++;
    }
  }

  return($nmissing,$missing);

}  


sub ParseParFile($thorn,$arrangement,$parfile,$config_data)
{
  my($thorn,$arrangement,$parfile,$config_data) = @_;
  my($line,$file,$processing_active);
  my($active,$desc);

  $file =  "arrangements/$arrangement/$thorn/test/$parfile";

  open (IN, "<$file") || die "Can not open $file";
    
  $processing_active = 0;
    
  # Give a default test name in case none is specified in the parameter file.
  $desc = "$arrangement/$thorn/test/$parfile";
    
  $active = "";

  while (<IN>)
  {
    $line = $_;
      
    if($processing_active == 1)
    {
      if($line =~ m/(.*)\"/)
      {
        $active .= " ".$1;
        $processing_active = 0;
      }
      else
      {
        $active .= " ".$line;
      }
    }
    elsif ($line =~ m/^\s*\!\s*DESC(RIPTION)?\s*\"(.*)\"\s*$/i)
    {
      $desc = $2;
    }
    elsif ($line =~ m/^\s*ActiveThorns\s*=\s*\"(.*)\"/i)
    {
      $active .= " ".$1;
    }
    elsif($line =~ m/^\s*ActiveThorns\s*=\s*\"(.*)/i)
    {
      $active .= " ".$1;
      $processing_active = 1;
    }
  }
  close IN;

  return($active,$desc);

}

sub ParseTestConfigs
{
    my($testdata,$config_data) = @_;

    foreach $thorn (split(" ",$testdata->{"THORNS"}))
    {
	$arrangement = $testdata->{"$thorn ARRANGEMENT"};

	$config = "$config_data->{\"CCTK_DIR\"}${sep}arrangements${sep}$arrangement${sep}$thorn${sep}test${sep}config";
	if (-e $config)
	{
	    $global = 1;
	    open (CONFIG,"< $config");
	    while (<CONFIG>)
	    {
		# Skip comment lines
		next if (/^\s*(\#.*|\!.*)$/);

		# Remember when we have moved off the global section
		if (/(.*):\s*/)
		{
		    $global = 0;
		    $test = $1;
		    next;
		}

		# Parse tokens
		if (/^\s*([^\s]*)\s(.*)$/)
		{
		    $token = $1;
		    $value = $2;
		}
		else
		{
		    print "  Unrecognised line in config file for thorn $thorn\n";
		}

		if ($token =~ /EXTENSIONS/)
		{
		    $testdata->{"EXTENSIONS"} .= "$value ";
		}
		else
		{
		    print "  Unrecognised token $token in config file for thorn $thorn\n";
		}
	    }
	}
    }

    return $testdata;
}


sub FindTestArchiveFiles
{
    my($testdata) = @_;
    my($thorn,$test);
    
    foreach $thorn (split(" ",$testdata->{"THORNS"}))
    {
	foreach $test (split(" ",$testdata->{"$thorn TESTS"}))
	{
	    $dir = "$testdata->{\"$thorn TESTSDIR\"}/$test";
	    ($testdata->{"$thorn $test UNKNOWNFILES"},$testdata->{"$thorn $test DATAFILES"}) = &FindFiles($dir,$testdata);
	    $testdata->{"$thorn $test NDATAFILES"} = scalar(split(" ",$testdata->{"$thorn $test DATAFILES"}));
	}
    }
    return $testdata;
}


sub FindTestParameterFiles
{
  my($testdata,$config_data) = @_;
  my($config,$config_dir);
  my($thorn);

  $config      = $config_data->{"CONFIG"};
  $configs_dir = $config_data->{"CONFIGSDIR"};
  $sep         = $config_data->{"SEPARATOR"};

  open (AT, "< $configs_dir${sep}$config${sep}ThornList") || print "Cannot find ThornList for $config";

  while (<AT>) 
  {
    next if (/^\s*(\#.*|\!.*)$/);

    /^\s*([^\s]*)\s*/;

    $fullthorn = $1;
    $testdata->{"FULL"} .= "$fullthorn ";
    
    $fullthorn =~ m:^\s*([^\s]*)/([^\s]*)\s*:;
    
    $thorn = $2;
    $testdata->{"THORNS"} .= "$thorn ";
    $testdata->{"$thorn ARRANGEMENT"} .= "$1";
    
    if ($testdata->{"ARRANGEMENTS"} !~ m:\s$1\s:)
    {
      $testdata->{"ARRANGEMENTS"} .= "$1 ";
    }
    
    $thorntestdir = "$config_data->{\"CCTK_DIR\"}${sep}arrangements${sep}$fullthorn${sep}test";
    
    if (-d $thorntestdir) 
    {
      $testdata->{"$thorn TESTSDIR"} = $thorntestdir;
      
      chdir $thorntestdir;
      
      while ($file=<*.par>)
      {
	$file =~ m:^(.*)\.par$:;
	$filedir = $1;
	if (-d $filedir)
	{
	  $testdata->{"$thorn TESTS"} .= "$filedir ";
	  $testdata->{"$thorn NTESTS"}++;
	}
	else
	{
	  &RunTestWarn(1,"Parameter file $filedir in thorn $thorn but no output directory");
	}
      }
    }
  }
  chdir $config_data->{"CCTK_DIR"};

  close AT;

  return $testdata;
}

sub RunTestWarn
{
    my($level,$message) = @_;
    return;
}

sub FindExecutionDetails
{
  my($config_data) = @_;
  my($config,$dir,$sep,$defns,$defexename,$executable);
  
  $config = $config_data->{"CONFIG"};
  $sep = $config_data->{"SEPARATOR"};

  # Check the name and directory of executable
  $defns = "$config_data->{\"CONFIGSDIR\"}${sep}$config${sep}config_data${sep}make.config.defn";

  $defexename = "cactus_$config";

  if (-e "$defns")
  {
    open(DEFNS,"<$defns");
    while(<DEFNS>)
    {
      if (/EXE\s*=\s*([\w_-]+)/)
      {
        $defexename = $1;
      }
      if (/EXEDIR\s*=\s*([\w_-]+)/)
      {
        $defexedirname = $1;
      }
    }
    close(DEFNS);
  }

  $executable = &defprompt("  Enter executable name (relative to Cactus home dir)","exe$sep$defexename");

  if (! (-e "$config_data->{\"CCTK_DIR\"}$sep$executable"))
  {
    if (-e "$config_data->{\"CCTK_DIR\"}$sep$executable.exe")
    {
      $executable .= ".exe";
    }
    else
    {
      die "Cannot locate $executable";
    }
  }

  $config_data->{"EXE"} = "$config_data->{\"CCTK_DIR\"}$sep$executable";

  $config_data = &FindRunCommand($config_data);

  return $config_data;
}

sub FindRunCommand
{
  my($config_data) = @_;
  my($command,$numprocs);

  # Look to see if MPI is dfined
  if (ParseExtras($config_data))
  {
    $numprocs = &defprompt("  Enter number of processors","2");
    $command = &defprompt("  Enter command to run executable","mpirun -np $numprocs ");
    $config_data->{"MULTIPROCESSOR"} = $numprocs;
  }
  else
  {
    $command = &defprompt("  Enter command to run executable"," ");
    $config_data->{"MULTIPROCESSOR"} = 0;
  }
  
  $config_data->{"COMMAND"} = $command;
  
  return $config_data;
}

sub defprompt 
{
  my ($pr, $de) = @_;
  my ($res);
  
  if ($config_data->{"PROMPT"} eq "no")
  {
    $res = $de;
  }
  else
  {
    print "$pr [$de] \n";
    print "   --> ";
  
    $res = <STDIN> if ($prompt eq "yes");
    if ($res =~ m/^$/) 
    {
      $res = $de;
    }
    elsif ($res =~ m/^ $/)
    {
      $res = "";
    }
    $res =~ s/\n//;
    print "\n";
  }
  return $res;
}

sub ParseExtras
{
  my($config_data) = @_;
  my($mpi,$dir,$sep,$extradir);

  $dir = $config_data->{"CCTK_DIR"};
  $sep = $config_data->{"SEPARATOR"};
  $config = $config_data->{"CONFIG"};

  $extradir = "$dir${sep}configs${sep}$config${sep}config-data${sep}cctk_Extradefs.h";

  $mpi = 0;

  if (-e "$extradir")
  {
    open(EXTRA,"<$extradir");
    while(<EXTRA>)
    {
      if (/\#define CCTK_MPI/)
      {
        $mpi = 1;
      }
    }
    close(EXTRA);
  }

  return $mpi;
}

sub InitialiseTestData
{
  my($testdata);

  # Complete list of thorns: arrangement/thorn
  $testdata->{"FULL"} = "";

  $testdata->{"NNODATAFILES"} = 0;
  $testdata->{"NRUNNABLE"} = 0;
  $testdata->{"NUNRUNNABLE"} = 0;
  $testdata->{"RUNNABLETHORNS"} = "";
  $testdata->{"UNRUNNABLETHORNS"} = "";
  $testdata->{"RUNNABLEARRANGEMENTS"} = "";
  $testdata->{"UNRUNNABLEARRANGEMENTS"} = "";

  return $testdata;
}

sub InitialiseRunData
{
  my(%runconfig);

  $runconfig{"ABSTOL"} = 1e-12;
  $runconfig{"RELTOL"} = 0;

  return %runconfig;
}

sub PrintDataBase
{
  my($database) = @_;
  my($field);
  
  foreach $field ( sort keys %$database )
  {
    print "$field has value\n   $database->{$field}\n";
  }
}

sub CleanDir
{
  my($dir) = @_;

  opendir (DIR, $dir);
  @list = (grep (/.+\..+/, readdir (DIR)));
  foreach $entry (@list)
  {
    unlink "$dir/$entry";
  }
  closedir (DIR);
}

sub RunCactus
{
  my($output,$testname,$command) = @_;
  my($retcode);

  printf "\n  Issuing $command\n";

  $retcode = 0;
  open (CMD, "pwd; $command |");
  open (LOG, "> $testname.log");

  while (<CMD>)
  {
    print LOG if ($output =~ /log/);
    print STDOUT if ($output =~ /stdout/);

    if( /Cactus exiting with return code (.*)/)
    {
      $retcode = $1 + 0;
    }
  }
  close LOG;
  close CMD;

  $retcode = $? >> 8 if($retcode==0);

  print STDOUT "\n\n" if ($output =~ /stdout/);
 
  return $retcode;
}


sub fpabs
{
  my ($val) = $_[0];
  $val > 0 ? $val:-$val;
}


sub PrintHeader
{
  print <<EOT;

  -------------------------------
   Cactus Code Test Suite Tool
  -------------------------------

EOT
}


sub FindFiles
{
  my ($dir,$testdata) = @_;
  my ($unrecognizedfiles,$recognizedfiles, @tmp);

  $recognizedfiles="";
  $unrecognizedfiles="";
  
  opendir (DIR, $dir);
  @tmp = readdir (DIR);
  closedir (DIR);

  foreach $f (@tmp) 
  {
      $f =~ m:.*\.([^\s\.]+)\s*$:;
      $extension = $1;
      
      if ($f !~ /^(\.\#.*|\.|\.\.|.*\.par|CVS|.*~)$/)
      {
	if ($extension =~ /.+/ && $testdata->{"EXTENSIONS"} =~ /\b$extension\b/)
	{
	  $recognizedfiles .= " $f "; 
	}
	else
	{
	  $unrecognizedfiles .= " $f";
	}
      }
  }
  
  return ($unrecognizedfiles,$recognizedfiles);
}


sub WriteFullResults
{
  my ($rundata,$testdata,$config_data) = @_;
  my ($separator,$show_warnings);

  $separator1 = "========================================================================\n\n";
  $separator2 = "------------------------------------------------------------------------\n\n";

  $show_warnings = 1;

  if ($show_warnings)
  {
    print $separator2;
    print "  Warnings for configuration $config_data->{\"CONFIG\"}\n  --------\n\n";

    # Missing thorns for tests
    
    $message = "  Tests missed for lack of thorns:\n";
    $missingtests = 0;
    foreach $thorn (split(" ",$testdata->{"UNRUNNABLETHORNS"}))
    {
      foreach $parfile (split(" ",$testdata->{"$thorn UNRUNNABLE"}))
      {
        $message .= "\n    ".$parfile." in ". $thorn."\n";
        $message .= "      (". $testdata->{"$thorn $parfile DESC"}.")\n";
        $message .= "      Missing: ".$testdata->{"$thorn $parfile MISSING"}."\n";
        $missingtests++;
      }
    }
    if ($missingtests > 0)
    {
      print "$message\n";
    }

    # Different numbers of test files

    $message =  "  Tests with different number of test files:\n\n";
    
    $extratests = 0;
    foreach $thorn (split(" ",$testdata->{"RUNNABLETHORNS"}))
    {
      foreach $parfile (split(" ",$testdata->{"$thorn RUNNABLE"}))
      {
        if ($rundata->{"$thorn $parfile NFILEEXTRA"}>0)
        {
          $extratests++;
          $message .= "    $thorn ($parfile)\n";
          $message .= "      Test created $rundata->{\"$thorn $parfile NFILEEXTRA\"} extra files: $testdata->{\"$thorn $parfile FILEEXTRA\"}\n";
        }
      }
    }
    if ($extratests > 0)
    {
      print "$message\n";
    }

  }

  print $separator2;

  print "  Testsuite Summary for configuration $config_data->{\"CONFIG\"}\n";
  print "  -----------------\n\n";

  print "  Suitable testsuite parameter files found in:\n\n";

  $tested = 0;
  $nottested = "";
  foreach $thorn (split(" ",$testdata->{"THORNS"}))
  {
    $num = scalar(split(" ",$testdata->{"$thorn RUNNABLE"}));
    if ($num > 0)
    {
      print "    $thorn [$num]\n";
      $tested++;
    }
    else
    {
      $nottested .= "\n    $thorn";
    }
  }

  print "\n";
  print "  Details:\n\n";
  foreach $thorn (split(" ",$testdata->{"THORNS"}))
  {
    $num = scalar(split(" ",$testdata->{"$thorn RUNNABLE"}));
    if ($num > 0)
    {
      print "    $thorn:\n";
      foreach $test (split(" ",$testdata->{"$thorn RUNNABLE"}))
      {
    print "      $test\n";
      }
    }
  }

  print "\n";
  if ($nottested)
  {
    print "  Thorns with no valid testsuite parameter files:\n";
    print "$nottested\n\n";
  }

  $unknown = 0;
  foreach $thorn (split(" ",$testdata->{"RUNNABLETHORNS"}))
  {
    if ($testdata->{"$thorn RUNNABLE"} !~ m:^\s*$:)
    {
      foreach $test (split(" ",$testdata->{"$thorn RUNNABLE"}))
      {
	$gotthorn = 0;
	if ($testdata->{"$thorn $test UNKNOWNFILES"})
	{
	  if (!$unknown)
	  {
	    print "  Thorns with unrecognized test output files:\n";
	    $unknown = 1;
	  }
	  
	  if (!$gotthorn)
	  {
	    print "    $thorn\n";
	    $gotthorn = 1;
	  }
	  print "       $test: $testdata->{\"$thorn $test UNKNOWNFILES\"}\n";
	}
      }
    }
  }

  print $separator2;

  print "  Run details for configuration $config_data->{\"CONFIG\"}\n\n";

  foreach $thorn (split(" ",$testdata->{"RUNNABLETHORNS"}))
  {
    if ($testdata->{"$thorn RUNNABLE"} !~ m:^\s*$:)
    {
      foreach $test (split(" ",$testdata->{"$thorn RUNNABLE"}))
      {
        print "      $thorn: $test\n         $rundata->{\"$thorn $test SUMMARY\"}\n";
      }
    }
    else
    {
      print "      No tests available\n";
    }
  }
  print "\n";

  print $separator1;

  print "  Summary for configuration $config_data->{\"CONFIG\"}\n\n";

  $total = $testdata->{"NUNRUNNABLE"}+$testdata->{"NRUNNABLE"};
  print "    Total available tests    -> $total\n";
  print "    Unrunnable tests         -> $testdata->{\"NUNRUNNABLE\"}\n";
  print "    Runnable tests           -> $testdata->{\"NRUNNABLE\"}\n";
  print "    Total number of thorns   -> ".scalar(split(" ",$testdata->{"THORNS"}))."\n";
  print "    Number of tested thorns  -> $tested\n";

  print "    Number of tests passed   -> $rundata->{\"NPASSED\"}\n";
  print "    Number passed only to\n";
  print "               set tolerance -> $rundata->{\"NPASSEDTOTOL\"}\n";
  print "    Number failed            -> $rundata->{\"NFAILED\"}\n";
  

  if ($rundata->{"NFAILED"})
  {
    print "\n  Tests failed:\n\n";
    foreach $thorn (split(" ",$testdata->{"THORNS"}))
    {
      foreach $file (split(" ",$rundata->{"$thorn FAILED"}))
      {
        print "    $file (from $thorn)\n";
      }
    }
  }

  print "\n";

  print $separator1;

}

sub ChooseTests
{
  my ($choice,$testdata) = @_;
  my ($count,$arrangement,@myarrs,$arrchoice,$thorn,$mythorns,$mytests);
  my ($testcount,$test,$thornchoice);

  if ($choice =~ m:^A:i)
  {
    print "  No runnable testsuites in arrangements: ";
    print "$testdata->{\"UNRUNNABLEARRANGEMENTS\"}\n\n";

    print "  Arrangements with runnable testsuites:\n";
    $count = 1;
    foreach $arrangement (split(" ",$testdata->{"RUNNABLEARRANGEMENTS"}))
    {
      printf ("   [%2d] $arrangement\n",$count);
      $myarrs[$count] = "$arrangement";
      $count++;
    }
    while (!$arrchoice or $arrchoice eq " ")
    {
      $arrchoice = &defprompt("  Choose arrangement by number:"," ");
    }

    print "  No runnable testsuites in thorns: ";
    foreach  $thorn (split(" ",$testdata->{"UNRUNNABLETHORNS"}))
    {
      if ($testdata->{"$thorn ARRANGEMENT"} =~ m:^$myarrs[$arrchoice]:)
      {
        print "$thorn ";
      }
    }
    print "\n\n";

    print "  Thorns in $myarrs[$arrchoice] with runnable testsuites:\n";
    $count = 1;
    foreach $thorn (split(" ",$testdata->{"RUNNABLETHORNS"}))
    {
      if ($testdata->{"$thorn ARRANGEMENT"} =~ m:^$myarrs[$arrchoice]:)
      {
        printf ("  [%2d] $thorn\n",$count);
        $mythorns[$count] = "$thorn";
        $count++;
      }
    }
    while (!$thornchoice or $thornchoice eq " ")
    {
      $thornchoice = &defprompt("  Choose thorn by number:"," ");
    }
    $testcount = 0;
    printf ("  [ 0] All tests\n");
    foreach $test (split(" ",$testdata->{"$mythorns[$thornchoice] RUNNABLE"}))
    {
      $testcount++;
      printf ("  [%2d] $test\n",$testcount);
      print "       $testdata->{\"$mythorns[$thornchoice] $test DESC\"}\n";
      $mytests[$testcount] = "$test";
    }
    $testchoice = &defprompt("  Choose test:","0");

    if ($testchoice == 0)
    {
      $ntests = $testcount;
      for ($i=0;$i<$testcount;$i++)
      {
	$returntests[2*$i]   = $mytests[$i+1];
	$returntests[2*$i+1] = $mythorns[$thornchoice];
      }
    }
    else
    {
      $ntests = 1;
      $returntests[0] = $mytests[$testchoice];
      $returntests[1] = $mythorns[$thornchoice];
    }
  }
  elsif ($choice =~ m:^T:i)
  {
    $count = 1;
    foreach $thorn (split(" ",$testdata->{"RUNNABLETHORNS"}))
    {
      printf ("  [%2d] $thorn\n",$count);
      $mythorns[$count] = "$thorn";
      $count++;
    }
    if ($count > 1)
    {
      while (!$thornchoice or $thornchoice eq " ")
      {
	$thornchoice = &defprompt("  Choose thorn:"," ");
      }
      $testcount = 0;
      printf ("  [ 0] All tests\n");
      foreach $test (split(" ",$testdata->{"$mythorns[$thornchoice] RUNNABLE"}))
      {
        $testcount++;
        printf ("  [%2d] $test\n",$testcount);
        print "       $testdata->{\"$mythorns[$thornchoice] $test DESC\"}\n";
        $mytests[$testcount] = "$test";
      }
      $testchoice = &defprompt("  Choose test:","0");      
      if ($testchoice == 0)
      {
        $ntests = $testcount;
        for ($i=0;$i<$testcount;$i++)
        {
          $returntests[2*$i]   = $mytests[$i+1];
          $returntests[2*$i+1] = $mythorns[$thornchoice];
        }
      }
      else
      {
        $ntests = 1;
        $returntests[0] = $mytests[$testchoice];
        $returntests[1] = $mythorns[$thornchoice];
      }
    }
  }

  return ($ntests,@returntests);
}


sub RunTest
{
  my ($output,$test,$thorn,$config_data,$testdata) = @_;
  my ($test_dir,$config);
  my ($retcode);

  $arrangement = $testdata->{"$thorn ARRANGEMENT"};

  $testdata->{"$thorn $test TESTRUNDIR"} = $config_data->{"TESTS_DIR"}.$sep.$config_data->{"CONFIG"}.$sep.$thorn;

  $testdata->{"$thorn $test TESTOUTPUTDIR"} = $testdata->{"$thorn $test TESTRUNDIR"}.$sep.$test;

  # Make any necessary directories
  &MakeTestRunDir($testdata->{"$thorn $test TESTRUNDIR"});

  $parfile = $test.".par";

  # Clean the output directory for this test
  &CleanDir($testdata->{"$thorn $test TESTOUTPUTDIR"});

  # Run the test from the test thorn directory
  chdir ($testdata->{"$thorn $test TESTRUNDIR"}) ;

  $cmd = "$config_data->{\"COMMAND\"} $config_data->{\"EXE\"} $testdata->{\"$thorn TESTSDIR\"}${sep}$parfile";
  $retcode = &RunCactus($output,$test,$cmd);
  chdir $config_data->{"CCTK_DIR"};

  # Deal with the error code
  if($retcode != 0)
  {
    print "Cactus exited with error code $retcode\n";
    print "Please check the logfile $testdata->{\"$thorn $test TESTRUNDIR\"}$sep$test.log\n\n";
    $testdata->{"$thorn FAILED"} .= "$parfile ";
    $testdata->{"NFAILED"}++;
  }

  return $testdata;
}

sub CompareTestFiles
{
  my ($test,$thorn,$runconfig,$rundata,$config_data,$testdata) = @_;
  my ($test_dir,$file,$newfile,$oldfile,$maxdiff);
  my ($vmaxdiff,$tmaxdiff,$numlines);

  $test_dir = $testdata->{"$thorn $test TESTOUTPUTDIR"};
  
  # Add new output files to database
  ($rundata->{"$thorn $test UNKNOWNFILES"},$rundata->{"$thorn $test TESTFILES"}) = &FindFiles("$test_dir",$testdata);
  $rundata->{"$thorn $test NUNKNOWNFILES"} = scalar(split(" ",$rundata->{"$thorn $test UNKNOWNFILES"}));
  $rundata->{"$thorn $test NTESTFILES"} = scalar(split(" ",$rundata->{"$thorn $test TESTFILES"}));

  $rundata->{"$thorn $test NFAILWEAK"}=0;
  $rundata->{"$thorn $test NFAILSTRONG"}=0;

  if ($rundata->{"$thorn $test NTESTFILES"}) 
  {
      # Compare each file in the archived test directory
      foreach $file (split(" ",$testdata->{"$thorn $test DATAFILES"})) 
      {
	  $newfile = "$test_dir$sep$file"; 
	  $oldfile = "$testdata->{\"$thorn TESTSDIR\"}${sep}${test}${sep}$file";
	  
	  $rundata->{"$thorn $test $file NINF"}=0;
	  $rundata->{"$thorn $test $file NNAN"}=0;
	  $rundata->{"$thorn $test $file NINFNOTFOUND"}=0;
	  $rundata->{"$thorn $test $file NNANNOTFOUND"}=0;
	  $rundata->{"$thorn $test $file NFAILSTRONG"}=0;
	  $rundata->{"$thorn $test $file NFAILWEAK"}=0;

	  if ( -e $newfile && -s $newfile && -s $oldfile)
	  {
	      open (INORIG, "<$oldfile") || print "Warning: Archive file $oldfile not found";
	      open (INNEW,  "<$newfile") || print "Warning: Test file $newfile not found";
	      	      
	      undef(@maxdiff);
	      undef(@diffvals);
	      undef(@oldvals);
	      undef(@newvals);
	      undef(@valmax);
	      
	      $numlines = 0;
	      
	      while ($oline = <INORIG>) 
	      {
		  $nline = <INNEW>;
		  
		  next if (($oline =~ /^\s*["\#]/) && ($nline =~ /^\s*["\#]/));
		  $numlines++;
		  
		  # Now lets see if they differ.
		  if (!("\U$nline" eq "\U$oline")) 
		  {
		      
		      # Check differences 
		      if (($nline !~ /(nan|inf)/i) && ($oline !~ /(nan|inf)/i))
		      {
			  
			  # This is the new comparison (subtract last two numbers)
			  @newvals = split(' ',$nline);
			  @oldvals = split(' ',$oline);
			  
			  $nnew = scalar(@newvals);
			  $nold = scalar(@oldvals);
			  
			  # Make sure that floating point numbers have 'e' if exponential.
			  $allzero = 1;
			  for ($count = 0; $count < $nold; $count++)
			  {
			      $newvals[$count] =~ s/[dD]/e/; 
			      $oldvals[$count] =~ s/[dD]/e/; 
			      $diffvals[$count] = abs($newvals[$count] - $oldvals[$count]);
			      if ($allzero == 1)
			      {
				  $allzero = 0 if ($diffvals[$count] > 0);
			      }
			  }
			  
			  if ($allzero == 0) 
			  {
			      # They diff. But do they differ strongly?
			      $rundata->{"$thorn $test $file NFAILWEAK"}++;
			      
			      if (!$runconfig->{"$thorn $test ABSTOL"})
			      {
				  $abstol = $runconfig->{"ABSTOL"};
			      }
			      else
			      {
				  $abstol = $runconfig->{"$thorn $test ABSTOL"};
			      }
			      
			      if (!$runconfig->{"$thorn $test RELTOL"})
			      {
				  $reltol = $runconfig->{"RELTOL"};
			      }
			      else
			      {
				  $reltol = $runconfig->{"$thorn $test RELTOL"};
			      }
			      
			      $allunder = 1;
			      for ($count = 0; $count < $nold; $count++)
			      {
				  $vreltol[$count] = $reltol*&max(abs($oldvals[$count]),abs($newvals[$count]));
				  $vtol[$count] = &max($abstol,$vreltol[$count]);
				  if ($allunder == 1)
				  {
				      if ($diffvals[$count] >= $vtol[$count])
				      {
					  $allunder = 0;
				      }
				      
				  }
			      }
			      
			      unless ($allunder == 1) 
			      {
				  $rundata->{"$thorn $test $file NFAILSTRONG"}++;
			      }
			      
			      # store difference for strong failures
			      for ($count = 0; $count < $nold; $count++)
			      {
				  $maxdiff[$count] = &max($maxdiff[$count],$diffvals[$count]);
				  $valmax[$count] = &max(abs($oldvals[$count]),abs($newvals[$count]));
			      }
			  }
		      }
		      # Check against nans
		      elsif ($nline =~ /nan/i && $oline !~ /nan/i)
		      {
			  $rundata->{"$thorn $test $file NNAN"}++;
			  $rundata->{"$thorn $test $file NFAILWEAK"}++;
			  $rundata->{"$thorn $test $file NFAILSTRONG"}++;
		      }
		      # Check against inf
		      elsif ($nline =~ /inf/i && $oline !~ /inf/i)
		      {
			  $rundata->{"$thorn $test $file NINF"}++;
			  $rundata->{"$thorn $test $file NFAILWEAK"}++;
			  $rundata->{"$thorn $test $file NFAILSTRONG"}++;
		      }
		      elsif ($oline =~ /nan/i)
		      {
			  $rundata->{"$thorn $test $file NNANNOTFOUND"}++;
			  $rundata->{"$thorn $test $file NFAILWEAK"}++;
			  $rundata->{"$thorn $test $file NFAILSTRONG"}++;
		      }
		      elsif ($oline =~ /inf/i)
		      {
			  $rundata->{"$thorn $test $file NINFNOTFOUND"}++;
			  $rundata->{"$thorn $test $file NFAILWEAK"}++;
			  $rundata->{"$thorn $test $file NFAILSTRONG"}++;
		      }
		      else
		      {
			  print "TESTSUITE ERROR: Didn't catch case in CompareFiles\n";
		      }
		  } # if
	      } #while
	      
	  }
	  elsif (!-e $newfile && -s $oldfile)
	  {
	      print "     $file in archive but not created in test\n";
	      $rundata->{"$thorn $test NFAILWEAK"}++;
	      $rundata->{"$thorn $test NFAILSTRONG"}++;
	  }
	  elsif (!-e $newfile && -z $oldfile)
	  {
	      print "     $file in archive but not created in test\n";
	      print "       ($file empty in archive)\n";
	      $rundata->{"$thorn $test NFAILWEAK"}++;
	      $rundata->{"$thorn $test NFAILSTRONG"}++;
	  }
	  elsif (-e $newfile && -s $oldfile && -z $newfile)
	  {
	      print "     $file is empty in test\n";
	      $rundata->{"$thorn $test NFAILWEAK"}++;
	      $rundata->{"$thorn $test NFAILSTRONG"}++;
	  }
	  elsif (-e $newfile && -z $oldfile && -z $newfile)      
	  {
	      print "     $file empty in both test and archive\n";
	  }
	  elsif (-e $newfile && -z $oldfile && -s $newfile)      
	  {
	      print "     $file is empty in archive but not in test\n";
	      $rundata->{"$thorn $test NFAILWEAK"}++;
	      $rundata->{"$thorn $test NFAILSTRONG"}++;
	  }
	  else
	  {
	      print "     TESTSUITE ERROR: $newfile not compared\n";
	  }
	  
	  for ($count = 1; $count <= $nold; $count++)
	  {
	      if ($maxdiff[$count])
	      {
		  $rundata->{"$thorn $test $file MAXABSDIFF $count"} = $maxdiff[$count];
		  if ($valmax[$count] > 0)
		  {
		      $rundata->{"$thorn $test $file MAXRELDIFF $count"} = $maxdiff[$count]/$valmax[$count];
		  }
		  else
		  {
		      print "ERROR: How did I get here, maximum difference is $maxdiff[$count] and maximum value if $valmax[$count] for $file\n";
		  }
	      }
	      else
	      {
		  $rundata->{"$thorn $test $file MAXABSDIFF $count"} = 0;
		  $rundata->{"$thorn $test $file MAXRELDIFF $count"} = 0;
	      }
	      
	  }
	  
	  $rundata->{"$thorn $test $file NUMLINES"} = $numlines;
	  
      }
  }
  else
  {
      print "  \n  No files created in test directory\n";
      $rundata->{"$thorn $test NFAILWEAK"} = $testdata->{"$thorn $test NDATAFILES"};
      $rundata->{"$thorn $test NFAILSTRONG"} = $testdata->{"$thorn $test NDATAFILES"};
  }

  return $rundata;

}

sub max
{
  my($f1,$f2) = @_;

  if ($f1 > $f2) 
  {
    $retval = $f1;
  }
  else
  {
    $retval = $f2;
  }

  return $retval;
}

sub ReportOnTest
{
  my($test,$thorn,$rundata,$testdata) = @_;
  my($file,$tmp,$summary,$buffer);

  $buffer = "";

  # Different lines in files
  foreach $file (split(" ",$testdata->{"$thorn $test DATAFILES"})) 
  {
    if ($rundata->{"$thorn $test $file NFAILWEAK"} != 0)
    {
      $rundata->{"$thorn $test NFAILWEAK"}++;
      if ($rundata->{"$thorn $test $file NFAILSTRONG"} == 0) 
      {
        $buffer .= "\n  - $file: differences below tolerance on $rundata->{\"$thorn $test $file NFAILWEAK\"} lines";
      }
      else 
      {
        $rundata->{"$thorn $test NFAILSTRONG"}++;
        $buffer .= "\n  - $file: substantial differences!\n";
        $buffer .= "      caught  $rundata->{\"$thorn $test $file NNAN\"} NaNs in new $file\n" if $rundata->{"$thorn $test $file NNAN"};
        $buffer .= "      did not reproduce  $rundata->{\"$thorn $test $file NNANNOTFOUND\"} NaNs from old $file\n" if $rundata->{"$thorn $test $file NNANNOTFOUND"};
        $buffer .= "      caught  $rundata->{\"$thorn $test $file NINF\"} Infs in new $file\n" if $rundata->{"$thorn $test $file NINF"};
        $buffer .= "      did not reproduce  $rundata->{\"$thorn $test $file NINFNOTFOUND\"} Infs from old $file\n" if $rundata->{"$thorn $test $file NINFNOTFOUND"};
        $buffer .= "      significant differences on $rundata->{\"$thorn $test $file NFAILSTRONG\"} (out of $rundata->{\"$thorn $test $file NUMLINES\"}) lines!\n";
        foreach $val (keys (%$rundata))
        {
          if ($val =~ /$thorn $test $file MAXABSDIFF (.*)$/)
          {
            $column = $1;
            if ($rundata->{"$thorn $test $file MAXABSDIFF $column"})
            {
              $buffer .= "      maximum absolute difference in column $column is $rundata->{\"$val\"}\n";
            }
          }
          elsif ($val =~ /$thorn $test $file MAXRELDIFF (.*)$/)
          {
            $column = $1;
            if ($rundata->{"$thorn $test $file MAXRELDIFF $column"})
            {
              $buffer .= "      maximum relative difference in column $column is $rundata->{\"$val\"}\n";
            }
          }
        }

        $tmp = $rundata->{"$thorn $test $file NFAILWEAK"} - $rundata->{"$thorn $test $file NFAILSTRONG"};
        if ($tmp)
        {
          $buffer .= "      (insignificant differences on $tmp lines)\n";
        } 
      }
    }
  }
  if ($buffer)
  {
      $buffer .= "\n";
  }
  print $buffer;

  # Give a warning if there were different files created

  # Look for files created by test not in archive 
  # (Note this is not so bad)
  foreach $file (split (" ",$rundata->{"$thorn $test TESTFILES"}))
  {
    $myfile = quotemeta($file);
    if ($testdata->{"$thorn $test DATAFILES"} !~ m:\b$myfile\b:)
    {
      print "            $file not in thorn archive\n";
      $rundata->{"$thorn $test NFILEEXTRA"}++;
      $rundata->{"$thorn $test FILEEXTRA"} .= " $file";
    }
  }

  # Look for files in archive which are not created in test
  # (Note this is bad)
  if ($testdata->{"$thorn $test NTESTFILES"})
  {
      foreach $file (split (" ",$testdata->{"$thorn $test DATAFILES"}))
      {
	  $myfile = quotemeta($file);
	  if ($rundata->{"$thorn $test TESTFILES"} !~ m:\b$myfile\b:)
	  {
	      print "            $file not created in test\n";
	      $rundata->{"$thorn $test NFILEMISSING"}++;
	      $rundata->{"$thorn $test FILEMISSING"} .= " $file";
	  }
      }
  }
  else
  {
      $rundata->{"$thorn $test NFILEMISSING"}++;
      $rundata->{"$thorn $test FILEMISSING"} = $rundata->{"$thorn $test DATAFILES"};
  }

  if (! $rundata->{"$thorn $test NFAILWEAK"})
  {
    $summary = "Success: $testdata->{\"$thorn $test NDATAFILES\"} files identical";
    printf("\n  $summary\n");
    $rundata->{"NPASSED"}++;
  }
  else
  {
    if (! $rundata->{"$thorn $test NFAILSTRONG"})
    {
      $summary = "Success: $testdata->{\"$thorn $test NDATAFILES\"} files compared, $rundata->{\"$thorn $test NFAILWEAK\"} differ in the last digits";
      printf "\n  $summary\n";
      $rundata->{"NPASSED"}++;
      $rundata->{"NPASSEDTOTOL"}++;
    }
    else
    {
      $summary = "Failure: $testdata->{\"$thorn $test NDATAFILES\"} files compared, $rundata->{\"$thorn $test NFAILWEAK\"} differ, $rundata->{\"$thorn $test NFAILSTRONG\"} differ significantly";
      printf "\n  $summary\n";
      $rundata->{"$thorn FAILED"} .= "$test ";
      $rundata->{"NFAILED"}++;
    }
  }
  $rundata->{"$thorn $test SUMMARY"} = $summary;
  printf ("\n");

  return $rundata;
}


sub ResetTestStatistics
{
  my($rundata,$testdata) = @_;

  $rundata->{"NFAILED"} = 0;
  $rundata->{"NPASSED"} = 0;
  $rundata->{"NPASSEDTOTOL"} = 0;
  foreach $thorn (split(" ",$testdata->{"THORN"}))
  {
    $rundata->{"$thorn TESTED"} = 0;
  }

  return $rundata;
}



sub ParseAllParameterFiles
{
  my($testdata) = @_;

  # Collect thorns needed for each testsuite
  foreach $thorn (split(" ",$testdata->{"THORNS"}))
  {
    $arr = $testdata->{"$thorn ARRANGEMENT"};

    foreach $testbase (split(" ",$testdata->{"$thorn TESTS"}))
    {
      
      $parfile = "$testbase.par";

      # Set ActiveThorns and Description for this Test
      ($active,$desc) = &ParseParFile($thorn,$arr,$parfile,$config_data);
      $testdata->{"$thorn $testbase ACTIVE"} = $active;
      $testdata->{"$thorn $testbase DESC"} = $desc;

      # Find any missing thorns for this test
      ($nmissing,$missing) = 
        &MissingThorns($testdata->{"$thorn $testbase ACTIVE"},
                       $testdata->{"THORNS"});
      
      # Set whether test is runnable or not
      if($nmissing == 0)
      {
        $testdata->{"$thorn RUNNABLE"} .= "$testbase ";
        $testdata->{"NRUNNABLE"}++;
        $testdata->{"$thorn TESTED"} = 1;
        $testdata->{"$thorn NRUNNABLE"}++;
      }
      else
      {
        $testdata->{"$thorn UNRUNNABLE"} .= "$testbase ";
        $testdata->{"$thorn $testbase MISSING"} .= "$missing";
        $testdata->{"NUNRUNNABLE"}++;
        $testdata->{"$thorn NUNRUNNABLE"}++;
      }
    }
    
    if ($testdata->{"$thorn NRUNNABLE"} > 0)
    {
      $testdata->{"RUNNABLETHORNS"} .= "$thorn ";
      if ($testdata->{"RUNNABLEARRANGEMENTS"} !~ m:\b$arr\s:)
      {
        $testdata->{"RUNNABLEARRANGEMENTS"} .= "$arr ";
      }
    }
    else
    {
      $testdata->{"UNRUNNABLETHORNS"} .= "$thorn ";
    }
  }

  # Last look for arrangements with no runnable tests

  foreach $arr (split(" ",$testdata->{"ARRANGEMENTS"}))
  {
    if ($testdata->{"RUNNABLEARRANGEMENTS"} !~ m:\b$arr\s:)
    {
      $testdata->{"UNRUNNABLEARRANGEMENTS"} .= "$arr ";
    }
  }

  return $testdata;
}



sub MakeTestRunDir
{  
  my($dir) = @_;

  $dir =~  m:^(.*)/([^/]*)/([^/]*)$:;
  mkdir ($1,0755);
  mkdir ("$1/$2",0755);
  mkdir ("$1/$2/$3",0755);

}

sub ViewResults
{
  my($test,$thorn,$runconfig,$rundata,$testdata) = @_;
  my($count,$choice,$myfile,@myfiles);

  if ($rundata->{"$thorn $test NTESTFILES"} && $rundata->{"$thorn $test NFAILSTRONG"})
  {
    while ($myfile !~ /^c/i)
    {
      undef ($choice);
      print "  Files which differ strongly:\n";
      $count = 1;
      foreach $file (split(" ",$testdata->{"$thorn $test DATAFILES"}))
      {
        if ($rundata->{"$thorn $test $file NFAILSTRONG"})
        {
          print "    [$count] $file\n";
          $myfiles[$count] = $file;
          $count++;
        }
      }

      $myfile = &defprompt("  Choose file by number or [c]ontinue","c");

      while ($myfile !~ /^[c]/i && $choice !~ /^[c]/i)
      {
	print "  File $myfiles[$myfile] of test $test for thorn $thorn\n";
        $choice = &defprompt("  Choose action [l]ist, [d]iff, [g]raph, [c]ontinue","c");

        if ($choice =~ /^l/i)
        {
          print "Archived file: $myfiles[$myfile]\n\n";
          open (ARCHIVE, "<$testdata->{\"$thorn TESTSDIR\"}/$test/$myfiles[$myfile]");
          while (<ARCHIVE>)
          {
            print;
          }
          close (ARCHIVE);
          
          print "\n\nNew file: $myfiles[$myfile]\n\n";

          open (TEST, "<$testdata->{\"$thorn $test TESTOUTPUTDIR\"}/$myfiles[$myfile]");
          while (<TEST>)
          {
            print;
          }
          close (TEST);
          print "\n";
        }
        elsif ($choice =~ /^d/i)
        {
          print "\n  Performing diff on  <archive> <test>\n\n";
          $command = "  diff $testdata->{\"$thorn TESTSDIR\"}/$test/$myfiles[$myfile] $testdata->{\"$thorn $test TESTOUTPUTDIR\"}/$myfiles[$myfile]\n";
          print "$command\n\n";
          system($command);
          print "\n";
        }
        elsif ($choice =~ /^g/i)
        {
          print "  Xgraph <archive> <test>\n\n";
          $command = "  xgraph $testdata->{\"$thorn TESTSDIR\"}/$test/$myfiles[$myfile] $testdata->{\"$thorn $test TESTOUTPUTDIR\"}/$myfiles[$myfile] &\n";
          print "  $command\n";
          system($command);
        }        
      }
    }
  }

  return;

}

1;
