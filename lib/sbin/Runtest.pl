#!/bin/perl -s
#
# Test Suite tool
# Version: $Header$


$sep = "/";

$prompt = shift;
$config = shift;;

$prompt =~ tr/A-Z/a-z/;

$tolerance = 13;

$ansinormal="";
$ansibold="";

if ($prompt ne "no") {
   $ansinormal  = "\033[0m";
   $ansibold    =  "\033[1m";
}

# Work out where the config directory is
if($ENV{"CONFIGS_DIR"})
{
  $configs_dir = $ENV{"CONFIGS_DIR"};
}
else
{
  $configs_dir = "configs";
}

$current_directory = `pwd`;
chop($current_directory);
$current_directory =~ s,^//([^/]+)/,$1:/,;

&print_header;

# Look to see if MPI is defined
$extra = "$current_directory${sep}configs${sep}$config${sep}config-data${sep}cctk_Extradefs.h";

$mpi = 0;
if (-e "$extra")
{
  open(EXTRA,"<$extra");
  while(<EXTRA>)
  {
    if (/\#define CCTK_MPI/)
    {
      $mpi = 1
    }
  }
}


$executable = &defprompt("Enter executable name (relative to Cactus home dir)","exe${sep}cactus_$config");

if ($mpi)
{
  $numprocs = &defprompt("Enter number of processors","2");
  $command = &defprompt("Enter command to run executable","mpirun -np $numprocs ");
}
else
{
  $command = &defprompt("Enter command to run executable"," ");
}
$tests = &defprompt("Run All tests or go to Menu","All");



# Get the active thorns list and test files with thorns 
$scratchdir = "$configs_dir$sep$config";
 
if (!open (AT, "< $scratchdir${sep}ThornList")) {
    print "Can't open $scratchdir/ThornList - no thorn tests";
} 
else 
{
  chdir "arrangements";

  while (<AT>) 
  {
    next if (m:^\#:);
    $name = $_;
    $name =~ s/\#(.*)$//g;
    $name =~ /^\s*([^\s]*)\s*/;
    $T = $1;
    @allthorns = (@allthorns, $T);

    $T =~ m:^.*/([^\s]*)\s*:;
    $database{"\U$T THORN\E"} = $1;
    $T =~ m:^\s*([^\s]*)/:;
    $database{"\U$T ARRANGEMENT"} = $1; 
    
    $number = 0;
    if (-d "$T${sep}test") 
    {
      $thisdir = `pwd`;
      chop($thisdir);
      chdir "$T${sep}test" || die "Unable to chdir to $T${sep}test";
      while ($file=<*.par>)
      {
	$database{"\U$T TESTFILE\E"} = $file;
	@testfiles = (@testfiles, $file);
	@testthorns = (@testthorns, $T);
	$number++;
      }
      chdir "../../.." || die "Unable to chdir to $thisdir";

    }
    $ntests{"$T"} = $number;
    $database{"\U$T NTESTS\E"} = $number;
  }

  chdir "..";
}

# Parse the parameter files for directives
$ntests = 0;
foreach $t (@testfiles) 
{
  $ntests++;
  $file =  "arrangements/$testthorns[$ntests-1]/test/$t";
  open (IN, "<$file") || die "Can not open $file";
  while (<IN>)
  {
    $line = $_;

    if ($line =~ /^\s*\!\s*DESC(RIPTION)?\s*\"(.*)\"\s*$/i)
    {
      $testnames{$ntests} = $2;
    }
    if ($line =~ /^\s*ActiveThorns\s*=\s*\"(.*)\"/i)
    {
      $activethorns[$ntests-1] = $1;
    }
  }
  close IN;
}

if ($tests =~ /All/) {

  # Run all parameter files
  $ntests=0;
  $number_failed=0;
  $number_zerofiles=0;
  $number_passed1=0;
  $number_passed2=0;
  $number_missed=0;

  foreach $t (@testfiles) 
  {
    $haveallthorns = 1;
    $active = $activethorns[$ntests];
    @at = split(' ',$active);
    foreach $th (@at)
    {
      $th = "\U$th";
      $foundit = 0;

      foreach $tthorn  (@allthorns)
      {
        $tthorn =~ m:.*/(.*)$:;
	$thornpart = "\U$1";
	if ($thornpart eq $th)
	{
	  $foundit = 1;
	}
      }      
      if (!$foundit)
      {
	$haveallthorns = 0;
      }
    }

    $thorn = $testthorns[$ntests];        
    $ntests++;

    if ($haveallthorns)
    {
      $havethorns{"$t"} = 1;
      &runtest($t,$thorn,$ntests);
    }
    else
    {
      $havethorns{"$t"} = 0;
    }
  }


# Show the statistics

  print "==================================================\n";
  print "All tests run for configuration $config\n\n";
  print "Tested: \n";
  foreach $thorn (keys %ntests)
  {
    
    if ($ntests{"$thorn"} > 0)
    {
      print "  $thorn [$ntests{\"$thorn\"}]\n";
    }
  }

  print "\n";
  print "  Tests run -> $ntests\n";
  print "  Number passed -> $number_passed1\n";
  if ($number_passed2 > 0)
  {
    print " (Number passed to only $tolerance digits -> $number_passed2)\n";
  }
  print "  Number failed -> $number_failed\n";

  if ($number_failed>0)
  {
    print "\n  Tests failed:\n";
    for ($i=0; $i<$number_failed;$i++)
    {
      print "    ". $which_failed[$i]." (from ". $thorn_failed[$i].")\n";
    }
  }
  if ($number_zerofiles > 0)
  {
      print "  Number with no output files -> $number_zerofiles\n";
  }
  print "==================================================\n\n";
} 

else 

{

# Show the parameter file menu

  $choice = test01;
  $ntests = 0;
  foreach $t (@testfiles) 
  {
    $ntests++;
    $t =~ m:([^${sep}]+).par$:;
  $num = $1; 
  $inp{$num} = $t;
  $testnum{$ntests} = $num;
  }
  while (!($choice =~ /^q/i) ) 
  {
    print "\n--- Menu ---\n";
    $sp = "     ";
    for ($i=1;$i<$ntests+1;$i++) {
      print "[$i] ".$testthorns[$i-1]." $testnum{$i}: \n      \"$testnames{$i}\"\n";
    }
    print "\n  Enter number of test to run (quit to end) : ";
    $choice = <STDIN> if ($prompt eq "yes");
    $choice =~ s/\n//;
    $choice =~ s/\s//;
    print "\n";
    $ip = $inp{$testnum{$choice}};
    $thorn = $testthorns[$choice-1];
    if (!($choice =~ m/^q/i || $choice =~ m/^\s*$/)) 
    {
      &runtest($ip,$thorn,$choice);
    }
    if (!($choice =~ m/^q/i))
    {
      print "  Hit return to continue ";
      $continue = <STDIN> if ($prompt eq "yes");
    }
  }

  print "\n";

}

sub runtest {
    my ($inpf,$inthorn,$num) = @_;

    # File name from thorn
    $inpf = "arrangements/$inthorn/test/$inpf";

    # Directory for output
    $tsttop = ".${sep}TEST${sep}$config";
    mkdir (TEST,0755);
    mkdir ($tsttop,0755);

    $tp = $inpf;
    $tp =~ s:^.*$sep::;
    $tp =~ s/.par//;
 
    $test_base_dir = $inpf;
    $test_base_dir =~ s:[^${sep}]*$::;

    print "Running $tp: $testnames{$num}\n";

    unlink(<$tsttop${sep}$tp${sep}*.*>);

    if (! (-e $current_directory$sep$executable))
    {
       if (-e "$current_directory$sep${executable}.exe")
       {
          $executable .= ".exe";
       }
       else
       {
          die "Cannot locate $executable";
       }
    }

    $cmd = "($command $current_directory$sep$executable $current_directory$sep$inpf)";

    chdir ($tsttop);

    printf "Issuing $cmd\n";
    $retcode = 0;
    open (CMD, "$cmd |");
    open (LOG, "> $tp.log");

    while (<CMD>) {
        print LOG;

        if( /Cactus exiting with return code (.*)/){
            $retcode = $1 + 0;
        }
    }
    close LOG;
    close CMD;
    $retcode = $? >> 8 if($retcode==0);
    
    chdir ("../..");
   

    if($retcode != 0)
    {
        print "${ansibold}Cactus exited with error code $retcode $ansinormal \n";
        print "Please check the logfile $tsttop$sep$tp.log\n\n";
	$number_failed++;
	@which_failed = (@which_failed,$tp); 
	@thorn_failed = (@thorn_failed,$inthorn);
        return;
    }
 
    $indir = $inpf;
    $indir =~ s:.par:${sep}:g;
    @oldout = <$indir${sep}*.*l>;
    $blewit = 0;
    $reallyblewit = 0;
    $nfiles = 0;

    foreach $file (@oldout) {
        $nfiles ++;
        $newfile = $file;
        $newfile =~ s:^.*${sep}([^${sep}]+)$:$1:;
        $newfile = "$tsttop$sep$tp$sep$newfile"; 
#       print "Comparing $file with $newfile\n";

        open (INORIG, "<$file");
        open (INNEW,  "<$newfile");
        $nblow = 0;
        $nrealblow = 0;
        while ($oline = <INORIG>) {
            $nline = <INNEW>;
            # Now lets see if they differ.
	    if ($nline =~ /nan/i)
	    {
	      print "****CAUGHT NAN in $newfile****\n";
	    }
            if (!($nline eq $oline)) {

# This is the new comparison (subtract last two numbers)
                ($t1,$v1) = split(' ', $nline);
                ($t2,$v2) = split(' ', $oline);
                $vdiff = abs($v1 - $v2);
                if ($vdiff > 0) {

                  # They diff. But do they differ strongly?
                  $nblow ++;

		  $exp = sprintf("%e",$vdiff);
		  $exp =~ s/^.*e-(\d+)/$1/;
		  unless ($exp >= $tolerance) {
                    $nrealblow++;
		  } 
		}
            }
        }
        if ($nblow != 0) {
            $blewit ++;
            $stripfile = $newfile;
            $stripfile =~ s:^.*${sep}(.*)$:$1:;
            if ($nrealblow == 0) {
                print "     $stripfile differs at machine precision (which is OK!)\n";
            } else {
                $reallyblewit ++;
                print "Substantial differences detected in $stripfile\n";
                print "     $newfile $file\n";
                print "     Differ on $nblow lines!\n";
            }
        }
    }
    if ($nfiles == 0) 
    {
        printf("  $ansibold WARNING: ZERO files compared ! $ansinormal \n");
	$number_zerofiles++;
    }
    elsif ($blewit == 0) 
    {
        printf("  $ansibold Test succeeded!$ansinormal $nfiles files identical\n");
	$number_passed1++;
    } 
    else 
    {
        if ($reallyblewit == 0) 
        {
            printf "\n  $ansibold Test passed to $tolerance figures:$ansinormal ".
                "$nfiles compared, $blewit files differ in the last digits\n";
	    $number_passed1++;
	    $number_passed2++;
        } else {
            printf "\n  $ansibold TEST FAILED!!:$ansinormal ".
                "$nfiles compared, $blewit files differ, $reallyblewit differ significantly\n";
	    $number_failed++;
	    @which_failed = (@which_failed,$tp); 
	    @thorn_failed = (@thorn_failed,$inthorn);
        }
    }
    printf ("\n\n");

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

sub fpabs {
    my ($val) = $_[0];
    $val > 0 ? $val:-$val;
}


sub print_header
{
  print <<EOT;

-------------------------------
 Cactus Code Test Suite Tool
-------------------------------

EOT
}
