# /usr/bin/perl -s

$sbin_dir = "lib/sbin";

require "$sbin_dir/MakeUtils.pl";

print "\n";

print "Type \"help\" at any prompt for a description on how\nto use this script\n\n";

print "Developmental thorns/arrangements are labelled (dev)\n\n";

print "________________________________________________________________________\n\n";

print "Checkout applications, arrangements or thorns? [arrangements] : ";
    
$which = <STDIN>;

if ($which =~ /^h/i)
{
    &print_help();
}
elsif ($which =~ /^t/i)
{
    &get_thorns();
}
elsif ($which =~ /^ap/i)
{
    &get_applications();
}
else
{ 
    &get_arrangements();
}

print "\nQuit or checkout more applications, arrangements or thorns [quit] : ";
$dowhat = <STDIN>;
if ($dowhat !~ /^[tpah]/i )
{
    print "All done!\n";
    exit;
}

$doit = 1;
while ()
{
    if ($dowhat =~ /^h/i)
    {
	&print_help();
    }
    elsif ($dowhat =~ /^t/i)
    {
	&get_thorns();
    }
    elsif ($dowhat =~ /^ap/i)
    {
	&get_applications();
    }
    else
    { 
	&get_arrangements();
    }

    print "\nQuit or checkout more applications, arrangements or thorns [quit] : ";

    $dowhat = <STDIN>;
    if ($dowhat !~ /^[thpa]/)
    {
	print "All done!\n";
	exit;
    }    
}

sub get_arrangements
{

    print "\nYou already have arrangements: \n\n";

    &buildthorns("arrangements/","arrangements");
    
    print "\nAvailable arrangements: \n";

    open(MODULES,"cvs co -s | ");
    
    $count = 0;
    while(<MODULES>)
    {
	if (/(\w*)\s*ARRANGEMENT(.*)?/)
	{
	  $count++;
	  $name{$count} = "$1";
	  $devlev{$count} = "$2";
	}
    }
    
    
    for ($i=1; $i<$count+1;$i++)
    {
        $extra = "";
        if ($devlev{$i} == 2) 
	{
	  $extra = "(dev)";
	}
	print "  [$i] $name{$i} $extra\n";
    }
    
    print "\n";
    
    print "Checkout arrangements [1-$count] : ";
    
    # Goto target arrangement directory 
    chdir arrangements || die "Could not find arrangements directory";

    $range = <STDIN>;
    if ($range =~ /^h/i)
    {
	&print_help();
    }
    elsif ($range =~ /^\s*$/)
    {
	$range = "1-$count";
    }

    while ($range =~/^([0-9]+(?:-[0-9]+)?),?/)
    {
	$range = $';
	$1 =~ /^([0-9]*)(-[0-9]*)?$/;
	$first = $1;
	if (!$2) 
	{$last=$1}
	else
        {$2=~/-([0-9]*)/; $last=$1}

	for ($i=$first; $i<$last+1; $i++)
	{
	    system("cvs -q checkout $name{$i}");
	}	
    }

    chdir("..") || die "Could not go back to Cactus home directory\n";

}



sub get_thorns
{
    print "\nYou already have thorns: \n\n";
    
    &buildthorns("arrangements/","thorns");
    
    print "\nAvailable thorns: \n";

    open(MODULES,"cvs -q co -s | ");
    
    $count = 0;
    while(<MODULES>)
    {
	if (/(\w*\/?\w*)\s*THORN([^\s])\s/)
	{
	    $count++;
	    $name{$count} = "$1";
	    $devlev{$count} = "$2";
	}
    }


    for ($i=1; $i<$count+1;$i++)
    {
        $extra = "";
        if ($devlev{$i} == "2") 
	{
	  $extra = "(dev)";
	}
	print "  [$i] $name{$i} $extra\n";
    }
    
    print "\n";
    
    print "Checkout thorns [1-$count] : ";
    
    # Goto target arrangement directory 
    chdir arrangements || die "Could not find arrangements directory\n";

    $range = <STDIN>;
    if ($range =~ /^h/i)
    {
	&print_help();
    }
    elsif ($range =~ /^\s*$/)
    {
	$range = "1-$count";
    }

    while ($range =~/^([0-9]+(?:-[0-9]+)?),?/)
    {
	$range = $';
	$1 =~ /^([0-9]*)(-[0-9]*)?$/;
	$first = $1;
	if (!$2) 
	{$last=$1}
	else
        {$2=~/-([0-9]*)/; $last=$1}
	
	for ($i=$first; $i<$last+1; $i++)
	{
	    system("cvs -q checkout $name{$i}");
	}
    }

    chdir ("..") || die "Could not return to Cactus home directory\n";
}




sub get_applications
{
  print "\nAvailable applications: \n";
  print "  [1] Example F90 wave equation evolver\n";
  print "  [2] Example F77 wave equation evolver\n";
  print "  [3] Example C   wave equation evolver\n";
  print "  [4] Benchmark (ADM)\n";
  print "\n";
    
  # Put number of applications here
  $count = 3;

  print "Checkout applications [1-$count] : ";
    
    # Goto target arrangement directory 
    chdir arrangements || die "Could not find arrangements directory\n";

    $range = <STDIN>;
    if ($range =~ /^h/i)
    {
	&print_help();
    }
    elsif ($range =~ /^\s*$/)
    {
	$range = "1-$count";
    }

    while ($range =~/^([0-9]+(?:-[0-9]+)?),?/)
    {
	$range = $';
	$1 =~ /^([0-9]*)(-[0-9]*)?$/;
	$first = $1;
	if (!$2) 
	{$last=$1}
	else
        {$2=~/-([0-9]*)/; $last=$1}
	
	 
	for ($i=$first; $i<$last+1; $i++)
	{
	  if ($i == 1)
	  {
	    # Checkout F90 WaveToy
	    print("\n");
	    print("Checking out WaveToyF90\n");
	    &CheckOut("CactusWave/WaveToyF90");
	    &CheckOut("CactusWave/IDScalarWave");
	    &CheckOut("CactusBase/Boundary");
	    &CheckOut("CactusBase/CartGrid3D");
	    &CheckOut("CactusBase/IOUtil");
	    &CheckOut("CactusBase/IOBasic");
	    &CheckOut("CactusPUGHIO/IOASCII");
	    &CheckOut("CactusPUGH/PUGH");
	    print("Completed checkout of application Wave F90\n");
	  }
	  elsif ($i == 2)
	  {
	    # Checkout F77 WaveToy
	    print("\n");
	    print("Checking out WaveToyF77\n");
	    &CheckOut("CactusWave/WaveToyF77");
	    &CheckOut("CactusWave/IDScalarWave");
	    &CheckOut("CactusBase/Boundary");
	    &CheckOut("CactusBase/CartGrid3D");
	    &CheckOut("CactusBase/IOUtil");
	    &CheckOut("CactusBase/IOBasic");
	    &CheckOut("CactusPUGHIO/IOASCII");
	    &CheckOut("CactusPUGH/PUGH");
	    print("Completed checkout of application Wave F77\n");
	  }	  
	  elsif ($i == 3)
	  {
	    # Checkout C WaveToy
	    print("\n");
	    print("Checking out WaveToyC\n");
	    &CheckOut("CactusWave/WaveToyC");
	    &CheckOut("CactusWave/IDScalarWave");
	    &CheckOut("CactusBase/Boundary");
	    &CheckOut("CactusBase/CartGrid3D");
	    &CheckOut("CactusBase/IOUtil");
	    &CheckOut("CactusBase/IOBasic");
	    &CheckOut("CactusPUGHIO/IOASCII");
	    &CheckOut("CactusPUGH/PUGH");
	    print("Completed checkout of application Wave C\n");
	  }	  
          elsif ($i == 4)
          {
            # Checkout ADM Benchmark
	    print("\n");
	    print("Checking out ADM\n");
	    system("cvs -q checkout CactusEinstein/ADM");
	    print("Checking out Einstein\n");
	    system("cvs -q checkout CactusEinstein/Einstein");
	    print("Checking out \n");
	    system("cvs -q checkout CactusBase/CartGrid3D");
	    print("Checking out IOUtil\n");
	    system("cvs -q checkout CactusBase/IOUtil");
	    print("Checking out IOASCII\n");
	    system("cvs -q checkout CactusPUGHIO/IOASCII");
	    print("Checking out PUGH\n");
	    system("cvs -q checkout CactusPUGH/PUGH");
	    print("Completed checkout of application Benchmark (ADM)\n");
          }
     
	}
    }

    chdir ("..") || die "Could not return to Cactus home directory\n";
}

sub print_help
{ 

    print "\nTo select arrangements or thorns for checking out from CVS, give\n";
    print "a comma separated list with the numbers of the thorns/arrangements.\n";
    print "Ranges can also be given, using a hyphen.\n";
    print "For example, to checkout thorns/arrangements 1,2,4,6,7,9 use:\n\n";
    print "   1-2,4,6-7,9\n\n";
}

sub CheckOut
{
  local($file) = @_;

  print("Checking out $file\n");
  system("cvs -z9 -q checkout $file");
}
