#!/bin/perl -s
#
# Test Suite tool.  Needs a few comments up here ...
# Version: $Id$
#

# Fix this for NT
$sep = "/";
$seps = "/";

$config = $ARGV[0];

$tolerance = 13;

$ansinormal  = "\033[0m";
$ansibold    =  "\033[1m";

print <<EOT;

-------------------------------
 Cactus Code Test Suite Tool
-------------------------------

EOT

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

# Look to see if MPI is defined
$extra = "$current_directory${sep}configs${sep}$config${sep}config-data${sep}cctk_extradefs.h";

$mpi = 0;
if (-e "$extra")
{
  open(EXTRA,"<$extra");
  while(<EXTRA>)
  {
    if (/\#define MPI/)
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
$tests = &defprompt("Run All tests or go to Menu",
                        "All");

# Get the active thorns list and test files with thorns 
$scratchdir = "$configs_dir$sep$config";
 
if (!open (AT, "< $scratchdir${sep}ThornList")) {
    print "Can't open $scratchdir/ThornList - no thorn tests";
} else {
    printf ("Reading Thorn List\n");
    while (<AT>) {
        @t = split(' ');
        foreach $T (@t) {
            if (-d "arrangements${sep}$T${sep}test") {
                @testfiles = (@testfiles, <arrangements${sep}$T${sep}test${sep}*.par>);
            }
        }
    }
}

$ntests = 0;
foreach $t (@testfiles) {
    $ntests++;
    open (IN, "< $t") || die "$t";
    $j = <IN>;
    $name = <IN>;
    close IN;
    $name =~ s/\#//;
    $name =~ s/\n//;
    $testnames{$ntests} = $name;
}

if ($tests =~ /All/) {
  $ntests=0;
  foreach $t (@testfiles) {
    $ntests++;
    &runtest($t,$ntests);
  }
} else {
  $choice = test01;
  $ntests = 0;
  foreach $t (@testfiles) {
    $ntests++;
    $t =~ m:${seps}([^${seps}]+).par$:;
    $num = $1;
    $inp{$num} = $t;
    $testnum{$ntests} = $num;
  }
  while (!($choice =~ /quit/i) ) {
    print "\n--- Menu ---\n";
    $sp = "     ";
    for ($i=1;$i<$ntests+1;$i++) {
      print "[$i] $testnum{$i}: $testnames{$i}\n";
    }
    print "\n  Enter number of test to run (quit to end) : ";
   $choice = <STDIN>;
    $choice =~ s/\n//;
    $choice =~ s/\s//;
    print "\n";
    $ip = $inp{$testnum{$choice}};
    if (!($choice =~ m/quit/i)) {
      &runtest($ip,$choice);
    }
  }
}

sub runtest {
    local ($inpf,$num) = @_;
    $tsttop = ".${sep}TEST";
    mkdir ($tsttop,0755);

    $tp = $inpf;
    $tp =~ s:^.*$seps::;
    $tp =~ s/.par//;
 
    $test_base_dir = $inpf;
    $test_base_dir =~ s:[^${seps}]*$::;

    $pretest  = "$test_base_dir$tp.pretest";
    $posttest = "$test_base_dir$tp.posttest";

    print "Running $tp: $testnames{$num}\n";

    unlink(<$tsttop${sep}$tp${sep}*.*>);

#   Run a pre-test script if it exists

    if( -x $pretest)
    {
        print "Running pre-test script $pretest\n";
        open (CMD, "(cd $tsttop; ..${sep}$pretest ..${sep}$test_base_dir) |");
        open (LOG, "> $tsttop${sep}$tp.prestep.log");
        while (<CMD>) {
            print LOG;
        }
        close LOG;
        close CMD;

        $retcode = $? >> 8;

        if($retcode > 0)
        {
            print "$pretest exited with error code $retcode\n";
            print "Aborting test\n";
            return;
        }
    }
 
    $cmd = "($command `pwd`$sep..$sep$executable ..$sep$inpf)";

    chdir ($tsttop);

    printf "Issuing $cmd\n";
    $retcode = 0;
    open (CMD, "$cmd |");
    open (LOG, "> ${sep}$tp.log");

    while (<CMD>) {
        print LOG;

        if( /Cactus exiting with return code (.*)/){
            $retcode = $1 + 0;
        }
    }
    close LOG;
    close CMD;
    
    chdir ("..");
   
    $retcode = $? >> 8 if($retcode==0);

    if($retcode != 0)
    {
        print "${ansibold}Cactus exited with error code $retcode $ansinormal \n";
        print "Please check the logfile $tsttop$sep$tp.log\n\n";
        return;
    }
 
    $indir = $inpf;
    $indir =~ s:.par:${seps}:g;
    @oldout = <$indir${sep}*.*l>;

    $blewit = 0;
    $reallyblewit = 0;
    $nfiles = 0;

    foreach $file (@oldout) {
        $nfiles ++;
        $newfile = $file;
        $newfile =~ s:^.*${seps}([^${seps}]+)$:\1:;
        $newfile = "$tsttop$sep$tp$sep$newfile"; 
#       print "Comparing $file with $newfile\n";

        open (INORIG, "<$file");
        open (INNEW,  "<$newfile");
        $nblow = 0;
        $nrealblow = 0;
        while ($oline = <INORIG>) {
            $nline = <INNEW>;
            # Now lets see if they differ.
            if (!($nline eq $oline)) {

# This is the new comparison (subtract last two numbers)
                ($t1,$v1) = split(' ', $nline);
                ($t2,$v2) = split(' ', $oline);
                $vdiff = abs($v1 - $v2);
                if ($vdiff > 0) {

                  # They diff. But do they differ strongly?
                  $nblow ++;

		  $exp = sprintf("%e",$vdiff);
		  $exp =~ s/^.*e-(\d+)/\1/;
		  #$exp = 0+$1;
		  unless ($exp >= $tolerance) {
                    #print "$v1 != $v2 ($vdiff [$exp] ) \n";
                    $nrealblow++;
		  } else {
                    #print "Difference is $vdiff [$exp]\n";
		  }
		}
            }
        }
        if ($nblow != 0) {
            $blewit ++;
            $stripfile = $newfile;
            $stripfile =~ s:^.*${seps}(.*)$:\1:;
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
    if ($nfiles == 0) {
        printf("  $ansibold WARNING: ZERO files compared ! $ansinormal \n");
    }
    if ($blewit == 0) {
        printf("  $ansibold Test succeeded!$ansinormal $nfiles files identical\n");
    } else {
        if ($reallyblewit == 0) {
            printf "\n  $ansibold Test passed to machine precision:$ansinormal ".
                "$nfiles compared, $blewit files differ in the last digit\n";
        } else {
            printf "\n  $ansibold TEST FAILED!!:$ansinormal ".
                "$nfiles compared, $blewit files differ, $reallyblewit differ significantly\n";
        }
    }
    printf ("\n\n");

#   Run a post-test script if it exists
    if( -x $posttest)
    {
        print "Running post-test script $posttest\n";
        open (CMD, "(cd $tsttop; ..$sep$posttest ..$sep$test_base_dir) |");
        open (LOG, "> $tsttop$sep$tp.posttest.log");
        while (<CMD>) {
            print LOG;
        }
        close LOG;
        close CMD;

        $retcode = $? >> 8;

        if($retcode > 0)
        {
            print "$posttest exited with error code $retcode\n";
            return;
        }
    }

}

sub defprompt {
    local ($pr, $de) = @_;
    local ($res);
    print "$pr [$de] \n";
    print "   --> ";
    $res = <STDIN>;
    if ($res =~ m/^\s*$/) {
        $res = $de;
    }
    $res =~ s/\n//;
    print "\n";
    return $res;
}

sub fpabs {
    local ($val) = @_[0];
    $val > 0 ? $val:-$val;
}
