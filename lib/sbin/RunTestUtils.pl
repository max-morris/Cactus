sub Configure
{
  my($config,$home_dir) = @_;
  my($configs_dir,$tests_dir);

  # Cactus home directory
  $config_data{"CCTK_DIR"} = $home_dir;

  # Cactus configurations directory
  if($ENV{"CONFIGS_DIR"})
  {
    $configs_dir = $ENV{"CONFIGS_DIR"};
  }
  else
  {
    $configs_dir = "configs";
  }
  $config_data{"CONFIGSDIR"} = $configs_dir;

  # Cactus test directory
  if ($ENV{"TESTS_DIR"})
  {
    $tests_dir = $ENV{"TESTS_DIR"};
  }
  else
  {
    $tests_dir = $home_dir."/TEST";
  }
  $config_data{"TESTS_DIR"} = $tests_dir;

  $config_data{"SEPARATOR"} = "/";
  $config_data{"CONFIG"} = $config;

  return %config_data;
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


sub ParseParFile($thorn,$parfile,%config_data)
{
  my($thorn,$parfile,%config_data) = @_;
  my($line,$file,$processing_active);
  my($active,$desc);

  $file =  "arrangements/$thorn/test/$parfile";

  open (IN, "<$file") || die "Can not open $file";
    
  $processing_active = 0;
    
  # Give a default test name in case non is specified in the parameter file.
  $desc = "$thorn/test/$t";
    
  while (<IN>)
  {
    $line = $_;
      
    if($processing_active == 1)
    {
      if($line =~ m/(.*)\"/)
      {
	$active .= $1;
	$processing_active = 0;
      }
      else
      {
	$active .= $line;
      }
    }
    elsif ($line =~ m/^\s*\!\s*DESC(RIPTION)?\s*\"(.*)\"\s*$/i)
    {
      $desc = $2;
    }
    elsif ($line =~ m/^\s*ActiveThorns\s*=\s*\"(.*)\"/i)
    {
      $active = $1;
    }
    elsif($line =~ m/^\s*ActiveThorns\s*=\s*\"(.*)/i)
    {
      $active = $1;
      $processing_active = 1;
    }
  }
  close IN;

  return($active,$desc);

}

sub ParseTests
{
  my(%config_data) = @_;
  my($scratch_dir);
  my($thorn);

  $config      = $config_data{"CONFIG"};
  $configs_dir = $config_data{"CONFIGSDIR"};
  $sep         = $config_data{"SEPARATOR"};

  if (!open (AT, "< $configs_dir${sep}$config${sep}ThornList")) 
  {
    print "Can't open $scratchdir/ThornList - no thorn tests";
  } 
  else 
  {
    while (<AT>) 
    {
      next if (m:^\s*$:);
      next if (m:^\#:);
      next if (m:^\!:);

      $name = $_;
      $name =~ s/\#(.*)$//g;
      $name =~ /^\s*([^\s]*)\s*/;
      $thorn = $1;
      $testdata{"FULL"} .= "$thorn ";
      
      $thorn =~ m:^.*/([^\s]*)\s*:;
      $testdata{"THORNS"} .= "$1 ";
      $thorn =~ m:^\s*([^\s]*)/:;
      if ($testdata{"ARRANGEMENTS"} !~ m:\s$1\s:)
      {
	$testdata{"ARRANGEMENTS"} .= "$1 ";
      }

      $thorntestdir = "$config_data{\"CCTK_DIR\"}${sep}arrangements${sep}$thorn${sep}test";

      if (-d $thorntestdir) 
      {
	$testdata{"$thorn PARFILESDIR"} = $thorntestdir;

	chdir $thorntestdir;

	while ($file=<*.par>)
	{
	  $file =~ m:^(.*)\.par$:;
	  $filedir = $1;
	  if (-d $filedir)
	  {
	    $testdata{"$thorn $filedir DATAFILES"} .= &FindFiles($filedir);
	    $testdata{"$thorn $filedir NDATAFILES"} = scalar(split(" ",$testdata{"$thorn $filedir DATAFILES"}));
	    $testdata{"$thorn PARFILES"} .= "$file ";
	    $testdata{"$thorn NUMBER"}++;
	  }
	}
      }
    }
    chdir $config_data{"CCTK_DIR"};
  }

  return %testdata;

}

sub ParseExecutable
{
  my(%config_data) = @_;
  
  $config = $config_data{"CONFIG"};
  $dir = $config_data{"THISDIR"};
  $sep = $config_data{"SEPARATOR"};

  # Check the name and directory of executable
  $defns = "$dir${sep}configs${sep}$config${sep}config_data${sep}make.config.defn";

  $defexename = "cactus_$config";

  if (-e "$defns")
  {
    open(DEFNS,"<$defns");
    while(<DEFNS>)
    {
      if (/EXE\s*=\s*([\w-_]+)/)
      {
	$defexename = $1;
      }
      if (/EXEDIR\s*=\s*([\w-_]+)/)
      {
	$defexedirname = $1;
      }
    }
    close(DEFNS);

  }

  $executable = &defprompt("  Enter executable name (relative to Cactus home dir)","exe$sep$defexename");

  if (! (-e $config_data{"CCTK_DIR"}.${sep}.$executable))
  {
    if (-e $config_data{"CCTK_DIR"}.${sep}.${executable}.".exe")
    {
      $executable .= ".exe";
    }
    else
    {
      die "Cannot locate $executable";
    }
  }

  return $config_data{"CCTK_DIR"}.${sep}.$executable;
}


sub defprompt 
{
  my ($pr, $de) = @_;
  my ($res);
  
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
  return $res;
}

sub ParseExtras
{
  my(%config_data) = @_;
  my($mpi,$dir,$sep,$extradir);

  $dir = $config_data{"CCTK_DIR"};
  $sep = $config_data{"SEPARATOR"};
  $config = $config_data{"CONFIG"};

  $extradir = "$dir${sep}configs${sep}$config${sep}config_data${sep}cctk_Extradefs.h";

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
  my(%testdata);

  # Complete list of thorns: arrangement/thorn
  $testdata{"FULL"} = "";

  $testdata{"TOLERANCE"} = 13;
  $testdata{"NFAILED"} = 0;
  $testdata{"NNODATAFILES"} = 0;
  $testdata{"NRUNNABLE"} = 0;
  $testdata{"NUNRUNNABLE"} = 0;
  $testdata{"RUNNABLETHORNS"} = "";
  $testdata{"UNRUNNABLETHORNS"} = "";
  $testdata{"RUNNABLEARRANGEMENTS"} = "";
  $testdata{"UNRUNNABLEARRANGEMENTS"} = "";

  return %testdata;
}

sub PrintDataBase
{
  my(%database) = @_;
  my($field);
  
  foreach $field ( sort keys %database )
  {
    print "$field has value $database{$field}\n\n";
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
  my($testname,$command) = @_;
  my($retcode);

  printf "  Issuing $command\n";
  $retcode = 0;
  open (CMD, "$command |");
  open (LOG, "> $testname.log");

  while (<CMD>)
  {
    print LOG;

    if( /Cactus exiting with return code (.*)/)
    {
      $retcode = $1 + 0;
    }
  }
  close LOG;
  close CMD;
  $retcode = $? >> 8 if($retcode==0);
 
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
  my ($dir) = @_;
  my ($files, @tmp);

  $files="";

  opendir (DIR, $dir);
  @tmp = grep (/\..+l$/, readdir (DIR));
  foreach $f (@tmp)
  {
    $files .= " $f";
  }
  closedir (DIR);
 
  return $files;
}

sub PrintDataBase
{
  my (%db) = @_;

  foreach $key (sort keys %db)
  {
    print "$key\n  $db{\"$key\"}\n";
  }
}


sub WriteFullResults
{
  my (%testdata) = @_;

  $separator = "==========================================================\n\n";

  $show_warnings = 1;

  if ($show_warnings)
  {
    print $separator;
    print "  Warnings for configuration $config\n  --------\n";

    # Missing thorns for tests
    
    $message = "  Tests missed for lack of thorns:\n\n";
    $missingtests = 0;
    foreach $thorn (split(" ",$testdata{"UNRUNNABLETHORNS"}))
    {
      foreach $parfile (split(" ",$testdata{"$thorn UNRUNNABLE"}))
      {
	$message .= "\n    ".$parfile." in ". $thorn."\n";
	$message .= "    (". $testdata{"$thorn $test DESC"}.")\n";
	$message .= "    Missing: ".$testdata{"$thorn $parfile MISSING"}."\n";
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
    foreach $thorn (split(" ",$testdata{"RUNNABLETHORNS"}))
    {
      foreach $parfile (split(" ",$testdata{"$thorn RUNNABLE"}))
      {
	if ($testdata{"$thorn $parfile NFILEEXTRA"}>0)
	{
	  $extratests++;
	  $message .= "    $thorn ($parfile)\n";
	  $message .= "      Test created $testdata{\"$thorn $parfile NFILEEXTRA\"} extra files: $testdata{\"$thorn $parfile FILEEXTRA\"}\n";
	}
      }
    }
    if ($extratests > 0)
    {
      print "$message\n";
    }

  }

  print $separator;

  print "  Testsuite Summary for configuration $config\n";
  print "  -----------------\n\n";

  print "  Suitable testsuite parameter files found in:\n\n";

  $tested = 0;
  $nottested = "";
  foreach $thorn (split(" ",$testdata{"FULL"}))
  {
    $num = scalar(split(" ",$testdata{"$thorn RUNNABLE"}));
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
  foreach $thorn (split(" ",$testdata{"FULL"}))
  {
    $num = scalar(split(" ",$testdata{"$thorn RUNNABLE"}));
    if ($num > 0)
    {
      print "    $thorn:\n";
      foreach $test (split(" ",$testdata{"$thorn RUNNABLE"}))
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

  print $separator;

  print "  Run details for configuration $config\n\n";

  foreach $thorn (split(" ",$testdata{"RUNNABLETHORNS"}))
  {
    if ($testdata{"$thorn RUNNABLE"} !~ m:^\s*$:)
    {
      foreach $test (split(" ",$testdata{"$thorn RUNNABLE"}))
      {
	print "      $thorn: $test\n         $testdata{\"$thorn $test SUMMARY\"}\n";
      }
    }
    else
    {
      print "      No tests available\n";
    }
  }
  print "\n";

  print $separator;

  print "  Summary for configuration $config\n\n";

  $total = $testdata{"NUNRUNNABLE"}+$testdata{"NRUNNABLE"};
  print "    Total available tests   -> $total\n";
  print "    Unrunnable tests        -> $testdata{\"NUNRUNNABLE\"}\n";
  print "    Runnable tests          -> $testdata{\"NRUNNABLE\"}\n";
  print "    Total number of thorns  -> ".scalar(split(" ",$testdata{"FULL"}))."\n";
  print "    Number of tested thorns -> $tested\n";

  print "    Number of tests passed  -> $number_passed1\n";
  print "    Number failed           -> $testdata{\"NFAILED\"}\n";
  

  if ($testdata{"NFAILED"})
  {
    print "\n  Tests failed:\n\n";
    foreach $thorn (split(" ",$testdata{"FULL"}))
    {
      foreach $file (split(" ",$testdata{"$thorn FAILED"}))
      {
        print "    $file (from $thorn)\n";
      }
    }
  }

  print "\n";

  print $separator;

}

sub ChooseTest
{
  my ($choice,%testdata) = @_;
  my ($count,$arrangement,@myarrs,$arrchoice,$thorn,$mythorns,$mytests);
  my ($testcount,$test,$thornchoice);

  if ($choice =~ m:^A:i)
  {

    print "  No runnable testsuites in arrangements: ";
    print "$testdata{\"UNRUNNABLEARRANGEMENTS\"}\n\n";

    print "  Arrangements with runnable testsuites:\n";
    $count = 1;
    foreach $arrangement (split(" ",$testdata{"RUNNABLEARRANGEMENTS"}))
    {
      printf ("   [%2d] $arrangement\n",$count);
      $myarrs[$count] = "$arrangement";
      $count++;
    }
    $arrchoice = &defprompt("  Choose arrangement by number:"," ");

    print "  No runnable testsuites in thorns: ";
    foreach  $thorn (split(" ",$testdata{"UNRUNNABLETHORNS"}))
    {
      if ($thorn =~ m:^$myarrs[$arrchoice]/:)
      {
	print "$thorn ";
      }
    }
    print "\n\n";

    print "  Thorns in $myarrs[$arrchoice] with runnable testsuites:\n";
    $count = 1;
    foreach $thorn (split(" ",$testdata{"RUNNABLETHORNS"}))
    {
      if ($thorn =~ m:^$myarrs[$arrchoice]/(.*):)
      {
	printf ("  [%2d] $1\n",$count);
	$mythorns[$count] = "$thorn";
	$count++;
      }
    }
    $thornchoice = &defprompt("  Choose thorn by number:"," ");
    $testcount = 1;
    foreach $test (split(" ",$testdata{"$mythorns[$thornchoice] RUNNABLE"}))
    {
      printf ("  [%2d] $test\n",$testcount);
      print "       $testdata{\"$mythorns[$thornchoice] $test DESC\"}\n";
      $mytests[$testcount] = "$test";
      $testcount++;
    }
    $testchoice = &defprompt("  Choose test:"," ");      
  }
  elsif ($choice =~ m:^T:i)
  {
    $count = 1;
    foreach $thorn (split(" ",$testdata{"RUNNABLETHORNS"}))
    {
      printf ("  [%2d] $thorn\n",$count);
      $mythorns[$count] = "$thorn";
      $count++;
    }
    if ($count > 1)
    {
      $thornchoice = &defprompt("  Choose thorn:"," ");
      $testcount = 1;
      foreach $test (split(" ",$testdata{"$mythorns[$thornchoice] RUNNABLE"}))
      {
	printf ("  [%2d] $test\n",$testcount);
	print "       $testdata{\"$mythorns[$thornchoice] $test DESC\"}\n";
	$mytests[$testcount] = "$test";
	$testcount++;
      }
      $testchoice = &defprompt("  Choose test:"," ");      
    }
  }
  return ($mytests[$testchoice],$mythorns[$thornchoice]);
}


sub RunTest
{
  my ($test,$inthorn,%testdata) = @_;
  my ($test_dir);
  my ($retcode);

  $inthorn =~ m:/(.*)$:;
  $mythorn = $1;

  # Directory for output
  $test_dir = "$config_data{\"TESTS_DIR\"}${sep}$config${sep}${mythorn}";

  mkdir ($config_data{"TESTS_DIR"},0755);
  mkdir ("$config_data{\"TESTS_DIR\"}${sep}$config",0755);
  mkdir ($test_dir,0755);

  $parfile = $test.".par";

  # Clean the output directory for this test
  &CleanDir("$test_dir${sep}$test");

  # Run the test from the test thorn directory
  chdir ($test_dir) || print "Error: $test_dir not found\n";

  $cmd = "$command $executable $testdata{\"$inthorn PARFILESDIR\"}${sep}$parfile";
  $retcode = &RunCactus($test,$cmd);
  chdir $config_data{"CCTK_DIR"};

  # Deal with the error code
  if($retcode != 0)
  {
    print "Cactus exited with error code $retcode\n";
    print "Please check the logfile $test_dir$sep$test.log\n\n";
    $testdata{"$inthorn FAILED"} .= "$parfile ";
    $testdata{"NFAILED"}++;
  }

  return %testdata;
}

sub CompareTestFiles
{
  my ($test,$inthorn,%testdata) = @_;
  my ($test_dir,$file);

  $inthorn =~ m:/(.*)$:;
  $mythorn = $1;

  $test_dir = "$config_data{\"TESTS_DIR\"}${sep}$config${sep}${mythorn}";
  
  # Add output files to database
  $testdata{"$inthorn $test TESTFILES"} = &FindFiles("$test_dir${sep}$test");

  $blewit = 0;
  $reallyblewit = 0;

  # Compare each file in the archived test directory

  foreach $file (split(" ",$testdata{"$inthorn $test DATAFILES"})) 
  {
    $newfile = "$test_dir$sep$test$sep$file"; 
    $oldfile = "$testdata{\"$inthorn PARFILESDIR\"}${sep}${test}${sep}$file";

    if ( -e $newfile)
    {
      open (INORIG, "<$oldfile") || print "Warning: Archive file $oldfile not found";
      open (INNEW,  "<$newfile") || print "Warning: Test file $newfile not found";

      $nblow = 0;
      $nrealblow = 0;
      $numnans=0;

      while ($oline = <INORIG>) 
      {
	$nline = <INNEW>;

	# Now lets see if they differ.
	if (!($nline eq $oline)) 
	{
      
	  # Check against nans
	  if ($nline =~ /nan/i)
	  {
	    $numnans++;
	    $nblow ++;
	    $nrealblow ++;
	  }

	  # Check against inf
	  elsif ($nline =~ /inf/i)
	  {
	    print "****CAUGHT INF in $newfile****\n";
	    $nblow ++;
	    $nrealblow ++;
	  }
	  else
	  {
	    # This is the new comparison (subtract last two numbers)
	    ($t1,$v1) = split(' ', $nline);
	    ($t2,$v2) = split(' ', $oline);
	    # Make sure that floating point numbers have 'e' if exponential.
	    $v1 =~ s/[dD]/e/; 
	    $v2 =~ s/[dD]/e/; 
	    
	    $vdiff = abs($v1 - $v2);
	    if ($vdiff > 0) 
	    {
	      
	      # They diff. But do they differ strongly?
	      $nblow ++;
	      
	      $exp = sprintf("%e",$vdiff);
	      $exp =~ s/^.*e-(\d+)/$1/;
	      if (!$testdata{"$inthorn $test TOLERANCE"})
	      {
		$tolerance = $testdata{"TOLERANCE"};
	      }
	      else
	      {
		$tolerance = $testdata{"$inthorn $test TOLERANCE"};
	      }
	      unless ($exp >= $tolerance) 
	      {
		$nrealblow++;
	      }
	    }
	  }
	} # if
      } #while

      if ($nblow != 0)
      {
	$blewit ++;
	$stripfile = $newfile;
	$stripfile =~ s:^.*${sep}(.*)$:$1:;
      if ($nrealblow == 0) 
        {
	  print "     $stripfile differs at machine precision (which is OK!)\n";
	}
      else 
	{
	  $reallyblewit ++;
	  print "  Substantial differences detected in $stripfile\n";
	  print "     $newfile $oldfile\n";
	  if ($numnans)
	  {
	    print "     Caught $numnans NaNs in $stripfile\n";
	    print "     Differ strongly on $nrealblow lines!\n";
	  }
	  $tmp = $nblow - $nrealblow;
	  if ($tmp)
	  {
	    print "     Differ weakly on $tmp lines!\n";
	  }
	  
	}
      }
    }
    else
    {
      print "     $newfile not there for comparison\n";
      $reallyblewit++;
      $blewit++;
    }
  }

  # Give a warning if there were different files created

  # Look for files created by test not in archive 
  # (Note this is not so bad)
  foreach $file (split (" ",$testdata{"$inthorn $test TESTFILES"}))
  {
    if ($testdata{"$inthorn $test DATAFILES"} !~ m:\b$file\b:)
    {
      print "            $file not in thorn archive\n";
      $testdata{"$inthorn $test NFILEEXTRA"}++;
      $testdata{"$inthorn $test FILEEXTRA"} .= " $file";
    }
  }

  # Look for files in archive which are not created in test
  # (Note this is bad)
  foreach $file (split (" ",$testdata{"$inthorn $test DATAFILES"}))
  {
    if ($testdata{"$inthorn $test TESTFILES"} !~ m:\b$file\b:)
    {
      print "            $file not created in test\n";
      $testdata{"$inthorn $test NFILEMISSING"}++;
      $testdata{"$inthorn $test FILEMISSING"} .= " $file";
    }
  }

  if ($blewit == 0)
  {
    $summary = "Success: $testdata{\"$inthorn $test NDATAFILES\"} files identical";
    printf("\n  $summary\n");
    $number_passed1++;
  }
  else
  {
    if ($reallyblewit == 0)
    {
      $summary = "Success (to $tolerance figures): $testdata{\"$inthorn $test NDATAFILES\"} compared, $blewit files differ in the last digits";
      printf "\n  $summary\n";
      $number_passed1++;
      $number_passed2++;
    }
    else
    {
      $summary = "Failure: ".
         "$testdata{\"$inthorn $test NDATAFILES\"} files compared, $blewit differ, $reallyblewit differ significantly";
      printf "\n  $summary\n";
      $testdata{"$inthorn FAILED"} .= "$test ";
      $testdata{"NFAILED"}++;
    }
  }
  $testdata{"$inthorn $test SUMMARY"} = $summary;
  printf ("\n");

  return %testdata;
}


sub ResetTestStatistics
{
  my(%testdata) = @_;

  foreach $thorn (split(" ",$testdata{"FULL"}))
  {
    $testdata{"$thorn TESTED"} = 0;
  }

  return %testdata;
}



sub ParseAllParameterFiles
{
  my(%testdata) = @_;

  # Collect thorns needed for each testsuite
  foreach $thorn (split(" ",$testdata{"FULL"}))
  {

    $thorn =~ m:^(.*)/:;
    $arr = $1;
    
    foreach $test (split(" ",$testdata{"$thorn PARFILES"}))
    {

      $test =~ m:^(.*)\.par$:;
      $testbase = $1;

      # Set ActiveThorns and Description for this Test
      ($active,$desc) = &ParseParFile($thorn,$test,%config_data);
      $testdata{"$thorn $test ACTIVE"} = $active;
      $testdata{"$thorn $testbase DESC"} = $desc;

      # Find any missing thorns for this test
      ($nmissing,$missing) = 
	&MissingThorns($testdata{"$thorn $testbase ACTIVE"},
		       $testdata{"THORNS"});
      
      # Set whether test is runnable or not
      if($nmissing == 0)
      {
	$testdata{"$thorn RUNNABLE"} .= "$testbase ";
	$testdata{"NRUNNABLE"}++;
	$testdata{"$thorn TESTED"} = 1;
	$testdata{"$thorn NRUNNABLE"}++;
      }
      else
      {
	$testdata{"$thorn UNRUNNABLE"} .= "$testbase ";
	$testdata{"$thorn $test MISSING"} .= "$missing";
	$testdata{"NUNRUNNABLE"}++;
	$testdata{"$thorn NUNRUNNABLE"}++;
      }
    }
    
    if ($testdata{"$thorn NRUNNABLE"} > 0)
    {
      $testdata{"RUNNABLETHORNS"} .= "$thorn ";
      if ($testdata{"RUNNABLEARRANGEMENTS"} !~ m:[\s^]$arr\s:)
      {
	$testdata{"RUNNABLEARRANGEMENTS"} .= "$1 ";
      }
    }
    else
    {
      $testdata{"UNRUNNABLETHORNS"} .= "$thorn ";
    }
  }

  # Last look for arrangements with no runnable tests

  foreach $arr (split(" ",$testdata{"ARRANGEMENTS"}))
  {
    if ($testdata{"RUNNABLEARRANGEMENTS"} !~ m:[\s^]$arr\s:)
    {
      $testdata{"UNRUNNABLEARRANGEMENTS"} .= "$arr ";
    }
  }

  return %testdata;
}



1;

