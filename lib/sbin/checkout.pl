# /usr/bin/perl -s

$sbin_dir = "lib/sbin";

require "$sbin_dir/MakeUtils.pl";

print "\n";

print "Type \"help\" at any prompt for a description on how\nto use this script\n\n";

print "Checkout applications, packages or thorns? [packages] : ";
    
$which = <STDIN>;

if ($which =~ /^h/i)
{
    &print_help();
}
elsif ($which =~ /^t/i)
{
    &get_thorns();
}
elsif ($which =~ /^a/i)
{
    &get_applications();
}
else
{ 
    &get_packages();
}

print "\nQuit or checkout more applications, packages or thorns [quit] : ";
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
    elsif ($dowhat =~ /^a/i)
    {
	&get_applications();
    }
    else
    { 
	&get_packages();
    }

    print "\nQuit or checkout more applications, packages or thorns [quit] : ";

    $dowhat = <STDIN>;
    if ($dowhat !~ /^[thpa]/)
    {
	print "All done!\n";
	exit;
    }    
}

sub get_packages
{

    print "\nYou already have packages: \n\n";

    &buildthorns("packages/","packages");
    
    print "\nAvailable packages: \n";

    open(MODULES,"cvs co -s | ");
    
    $count = 0;
    while(<MODULES>)
    {
	if (/(\w*)\s*PACKAGE/)
	{
	    $count++;
	    $name{$count} = $1;
	}
    }
    
    
    for ($i=1; $i<$count+1;$i++)
    {
	print "  [$i] $name{$i}\n";
    }
    
    print "\n";
    
    print "Checkout packages [1-$count] : ";
    
    # Goto target package directory 
    chdir packages || die "Could not find packages directory";

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
    
    &buildthorns("packages/","thorns");
    
    print "\nAvailable thorns: \n";

    open(MODULES,"cvs -q co -s | ");
    
    $count = 0;
    while(<MODULES>)
    {
	if (/(\w*\/?\w*)\s*THORN/)
	{
	    $count++;
	    $name{$count} = $1;
	}
    }


    for ($i=1; $i<$count+1;$i++)
    {
	print "  [$i] $name{$i}\n";
    }
    
    print "\n";
    
    print "Checkout thorns [1-$count] : ";
    
    # Goto target package directory 
    chdir packages || die "Could not find packages directory\n";

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
  print "\n";
    
  # Put number of applications here
  $count = 2;

  print "Checkout applications [1-$count] : ";
    
    # Goto target package directory 
    chdir packages || die "Could not find packages directory\n";

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
	    print("Checking out WaveToy\n");
	    system("cvs -q checkout CactusApplications/WaveToy");
	    print("Checking out Boundary\n");
	    system("cvs -q checkout CactusBase/Boundary");
	    print("Checking out CartGrid3D\n");
	    system("cvs -q checkout CactusBase/CartGrid3D");
	    print("Checking out IOUtil\n");
	    system("cvs -q checkout CactusBase/IOUtil");
	    print("Checking out IOASCII\n");
	    system("cvs -q checkout CactusPUGH/IOASCII");
	    print("Checking out PUGH\n");
	    system("cvs -q checkout CactusPUGH/PUGH");
	    print("Completed checkout of application Wave F90\n");
	  }
	  elsif ($i == 2)
	  {
	    # Checkout F77 WaveToy
	    print("\n");
	    print("Checking out WaveToyF77\n");
	    system("cvs -q checkout CactusApplications/WaveToyF77");
	    print("Checking out Boundary\n");
	    system("cvs -q checkout CactusBase/Boundary");
	    print("Checking out CartGrid3D\n");
	    system("cvs -q checkout CactusBase/CartGrid3D");
	    print("Checking out IOUtil\n");
	    system("cvs -q checkout CactusBase/IOUtil");
	    print("Checking out IOASCII\n");
	    system("cvs -q checkout CactusPUGH/IOASCII");
	    print("Checking out PUGH\n");
	    system("cvs -q checkout CactusPUGH/PUGH");
	    print("Completed checkout of application Wave F77\n");
	  }	  
	}
    }

    chdir ("..") || die "Could not return to Cactus home directory\n";
}

sub print_help
{ 

    print "\nTo select packages or thorns for checking out from CVS, give\n";
    print "a comma separated list with the numbers of the thorns/packages.\n";
    print "Ranges can also be given, using a hyphen.\n";
    print "For example, to checkout thorns/packages 1,2,4,6,7,9 use:\n\n";
    print "   1-2,4,6-7,9\n\n";
}
