#!/bin/perl -s
#
# Test Suite tool.  Needs a few comments up here ...
# Version: $Id$
#

$config = $ARGV[0];

if ($machine eq "CYGWIN_NT-4.0" || $machine eq "CYGWIN32_NT") {
    $sep = "\\";
    $seps = "\\\\";
} else {
    $sep = "/";
    $seps = "/";
}

$tolerance = 13;

$ansinormal  = "\033[0m";
$ansibold    =  "\033[1m";

print <<EOT;
-------------------------------
 Cactus Code Test Suite Tool
 Paul Walker + Joan Masso
-------------------------------

EOT

$command = &defprompt("Enter Command to run cactus code","./cactus_$config");

$tests = &defprompt("Run All tests or go to Menu",
                        "All");

# Get the active thorns list and test files with thorns 
$scratchdir = "configs$sep$config";
 
if (!open (AT, "< $scratchdir${sep}ThornList")) {
    print "Can't open $scratchdir/ThornList - no thorn tests";
} else {
    printf ("Reading Thorn List\n");
    while (<AT>) {
        @t = split(' ');
        foreach $T (@t) {
            if (-d "packages${sep}$T${sep}test") {
                @testfiles = (@testfiles, <packages${sep}$T${sep}test${sep}*.par>);
            }
        }
    }
}

foreach $t (@testfiles) {
    open (IN, "< $t") || die "$t";
    $j = <IN>;
    $name = <IN>;
    close IN;
    $name =~ s/\#//;
    $name =~ s/\n//;
    $testnames{$t} = $name;
}

if ($tests =~ /All/) {
    foreach $t (@testfiles) {
        &runtest($t);
    }
} else {
    $choice = test01;
    while (!($choice =~ /quit/i) ) {
        print "\n--- Menu ---\n";
        foreach $t (@testfiles) {
            $t =~ m:${seps}([^${seps}]+).par$:;
            $num = $1;
            $inp{$num} = $t;
            $sp = "     ";
            print "$num:$sp$testnames{$t}\n";
        }
        print "\n  Enter Choice, eg test01 (quit to end) : ";
        $choice = <STDIN>;
        $choice =~ s/\n//;
        $choice =~ s/\s//;
        print "\n";
        $ip = $inp{$choice};
        if (!($choice =~ m/quit/i)) {
            &runtest($ip);
        }
    }
}

sub runtest {
    local ($inpf) = @_;
    $tsttop = ".${sep}TEST";
    mkdir ($tsttop,0755);

    $tp = $inpf;
    $tp =~ s:^.*$seps::;
    $tp =~ s/.par//;
 
    if ($testnames{$inpf} =~ /SINGLE PROC/ && $nprocs > 1) {
        print "Test $tp: $testnames{$inpf}\n";
        print "  This test only runs on a single processor\n";
        print "$ansibold  Test skipped on $nprocs processors$ansinormal\n";
        print "\n\n";
        return;
    }

    if ($ENV{'CACTUS_PRECISION'} eq 'SINGLE') {
        if ($testnames{$inpf} !~ /SINGLE PRECISION/) {
            print "Test $tp: $testnames{$inpf}\n";
            print "  This test works only in double precision\n";
            print "$ansibold  Test skipped$ansinormal\n";
            print "\n\n";
            return;
        }
    } else {
        if ($testnames{$inpf} =~ /SINGLE PRECISION/) {
            print "Test $tp: $testnames{$inpf}\n";
            print "  This test works only in single precision\n";
            print "$ansibold  Test skipped$ansinormal\n";
            print "\n\n";
            return;
        }
    }

    $test_base_dir = $inpf;
    $test_base_dir =~ s:[^${seps}]*$::;

    $pretest  = "$test_base_dir$tp.pretest";
    $posttest = "$test_base_dir$tp.posttest";

    print "Running $tp: $testnames{$inpf}\n";

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
 
    $cmd = "(..$sep$command ..$sep$inpf)";

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
