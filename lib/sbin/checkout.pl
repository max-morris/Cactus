# /usr/bin/perl -s

$sbin_dir = "lib/sbin";

require "$sbin_dir/MakeUtils.pl";

# First check that CVS works
die "cvs not found\n" if (!&CVSFound);

# Print info
&PrintInfo;

# Choose CVS repository
$login = getlogin || getpwuid($<);
while (!$repository)
{    
  $repository = &choose_repository($login);
}

# Check can see repository
die "Repository not found\n" if (!&RepositoryExists($repository));

$finish = 0;
while (!$finish)
{   
    
  print "\nCheckout applications, arrangements, thorns or quit? [arrangements] : ";
    
  $which = <STDIN>;

  if ($which =~ /^q/i)
  {
    $finish = 1;
  }
  elsif ($which =~ /^h/i)
  {
    &print_help();
  }
  elsif ($which =~ /^t/i)
  {
    &get_thorns($repository);
  }
  elsif ($which =~ /^ap/i)
  {
    &get_applications($repository);
  }
  else
  { 
    &get_arrangements($repository);
  }

}

print "\nAll done!\n\n";
exit;


sub get_arrangements
{   
    my($repository) = @_;
    my(%info);
    my($arrangement);
    print "\nYou already have arrangements: \n\n";

    %info = &buildthorns("arrangements/","arrangements");
    
    foreach $arrangement (sort keys %info)
    {
      print "$arrangement\n";
    }

    print "\nAvailable arrangements: \n";

    open(MODULES,"cvs -d $repository co -s | ");
    
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
	  &CheckOut($name{$i},$repository);
	}	
    }

    chdir("..") || die "Could not go back to Cactus home directory\n";

}



sub get_thorns
{
    my($repository) = @_;
    my(%info);
    my($thorn);

    print "\nYou already have thorns: \n\n";
    
    %info = &buildthorns("arrangements/","thorns");
    
    foreach $thorn (sort keys %info)
    {
      print "$thorn\n";
    }

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
	  &CheckOut($name{$i},$repository);
	}
    }

    chdir ("..") || die "Could not return to Cactus home directory\n";
}




sub get_applications
{
  my($repository) = @_;

  print "\nAvailable applications: \n";
  print "  [1] Example F90 wave equation evolver\n";
  print "  [2] Example F77 wave equation evolver\n";
  print "  [3] Example C   wave equation evolver\n";
  print "  [4] Example C++ wave equation evolver\n";
  print "  [5] Benchmark (ADM)\n";
  print "\n";
    
  # Put number of applications here
  $count = 5;

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
	    &CheckOut("CactusWave/WaveToyF90,$repository");
	    &CheckOut("CactusWave/IDScalarWave,$repository");
	    &CheckOut("CactusBase/Boundary,$repository");
	    &CheckOut("CactusBase/CartGrid3D,$repository");
	    &CheckOut("CactusBase/IOUtil,$repository");
	    &CheckOut("CactusBase/IOBasic,$repository");
	    &CheckOut("CactusBase/Time,$repository");
	    &CheckOut("CactusPUGHIO/IOASCII,$repository");
	    &CheckOut("CactusPUGH/PUGH,$repository");
	    print("Completed checkout of application Wave F90\n");
	  }
	  elsif ($i == 2)
	  {
	    # Checkout F77 WaveToy
	    print("\n");
	    &CheckOut("CactusWave/WaveToyF77,$repository");
	    &CheckOut("CactusWave/IDScalarWave,$repository");
	    &CheckOut("CactusBase/Boundary,$repository");
	    &CheckOut("CactusBase/CartGrid3D,$repository");
	    &CheckOut("CactusBase/IOUtil,$repository");
	    &CheckOut("CactusBase/IOBasic,$repository");
	    &CheckOut("CactusBase/Time,$repository");
	    &CheckOut("CactusPUGHIO/IOASCII,$repository");
	    &CheckOut("CactusPUGH/PUGH,$repository");
	    print("Completed checkout of application Wave F77\n");
	  }	  
	  elsif ($i == 3)
	  {
	    # Checkout C WaveToy
	    print("\n");
	    &CheckOut("CactusWave/WaveToyC,$repository");
	    &CheckOut("CactusWave/IDScalarWave,$repository");
	    &CheckOut("CactusBase/Boundary,$repository");
	    &CheckOut("CactusBase/CartGrid3D,$repository");
	    &CheckOut("CactusBase/IOUtil,$repository");
	    &CheckOut("CactusBase/IOBasic,$repository");
	    &CheckOut("CactusBase/Time,$repository");
	    &CheckOut("CactusPUGHIO/IOASCII,$repository");
	    &CheckOut("CactusPUGH/PUGH,$repository");
	    print("Completed checkout of application Wave C\n");
	  }	  
	  elsif ($i == 4)
	  {
	    # Checkout C++ WaveToy
	    print("\n");
	    &CheckOut("CactusWave/WaveToyCXX,$repository");
	    &CheckOut("CactusWave/IDScalarWaveCXX,$repository");
	    &CheckOut("CactusBase/Boundary,$repository");
	    &CheckOut("CactusBase/CartGrid3D,$repository");
	    &CheckOut("CactusBase/IOUtil,$repository");
	    &CheckOut("CactusBase/IOBasic,$repository");
	    &CheckOut("CactusBase/Time,$repository");
	    &CheckOut("CactusPUGHIO/IOASCII,$repository");
	    &CheckOut("CactusPUGH/PUGH,$repository");
	    print("Completed checkout of application Wave C++\n");
	  }	  
          elsif ($i == 5)
          {
            # Checkout ADM Benchmark
	    print("\n");
	    &CheckOut("CactusEinstein/ADM,$repository");
	    &CheckOut("CactusEinstein/Einstein,$repository");
	    &CheckOut("CactusBase/Time,$repository");
	    &CheckOut("CactusBase/CartGrid3D,$repository");
	    &CheckOut("CactusBase/IOUtil,$repository");
	    &CheckOut("CactusPUGHIO/IOASCII,$repository");
	    &CheckOut("CactusPUGH/PUGH,$repository");
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
  my($file,$repository) = @_;

  print("Checking out $file\n");
  system("cvs -z9 -q -d $repository checkout $file");
}



sub choose_repository
{
  my($login) = @_;
  my($repository,$dowhat);

  open(IN,"<CVS/Root") || die "No file CVS/Root";
  $rep[1] = <IN>;
  chop($rep[1]);
  close IN;
  $rep[2]  = "Custom repository";
  
  # Get home directory from password file
  @dirs = getpwuid($<);
  $file = "@dirs[7]/.cvspass";

  if (open(CVSPASS,"<$file"))
  {
    $numinpass=0;
    while (<CVSPASS>)
    {
      $numinpass++;
      /^([^\s]*)\s[^\s]*/;
      $rep[2+$numinpass] = $1;
    }
  }
  else
  {
    print "Could not find $file/.cvspass containing CVS logins\n";
    print "Perhaps you need to login on this machine? Type \"help\"\n";
    print "at next prompt for details\n\n";
  }

  print "Choose CVS repository ? \n";
  print "  [1] This flesh repository $rep[1]\n";
  print "  [2] $rep[2]\n";
  for ($i=3;$i<=$numinpass+2;$i++)
  {
    print "  [$i] $rep[$i]\n";
  }
  print "\nRepository choice [1] : ";
  
  $dowhat = <STDIN>;
  
  if ($dowhat =~ /^$/) 
  {
    $dowhat = 1;
  } 
  elsif ($dowhat =~ /^h/i)
  {

    print "\nThe list of repositories includes all those repositories\n";
    print "into which you have logged in from this machine.\n\n";
    print "There are two official Cactus repositories on our server\n";
    print "at cvs.cactuscode.org, the latest stable release of the Cactus\n";
    print "flesh and arrangements is at\n\n";
    print "    /cactus\n\n";
    print "and the developmental version is at\n\n";
    print "    /cactusdevcvs\n\n";
    print "Checking out from the developmental version will give you all\n";
    print "the current bug fixes and new features, but is more unstable.\n\n";
    print "Both repositories can either be logged into anonymously, using\n\n";
    print "    cvs -d :pserver:cvs_anon\@cvs.cactuscode.org:/cactus[devcvs]\n\n";
    print "and the password \"anon\". Alternatively, if you have a login\n";
    print "at our repositories, using\n\n"; 
    print "    cvs -d :pserver:<name>\@cvs.cactuscode.org:/cactus[devcvs]\n\n";
    print "and your personal password will allow you to access or commit to\n";
    print "those modules to which you have privileges\n";
    print "\nReturn to continue\n";
    <STDIN>;
    
  }      
  if ($dowhat == 1)
  {
  }
  elsif ($dowhat == 2)
  {
    print "Custom repository : ";
    $rep[2] = <STDIN>;
  }

  if ($dowhat !~ /^h/i)
  { 
    print "Using repository $rep[$dowhat]\n";
  }
  
  return $rep[$dowhat];
  
}

sub PrintInfo
{
  print "\n";
  print "Type \"help\" at any prompt for a description on how\nto use this script\n\n";
  print "Developmental thorns/arrangements are labelled (dev)\n\n";
  print "________________________________________________________________________\n\n";
}    

sub CVSFound
{

  my($foundit);
  
  $foundif = 0;
  open(MODULES,"cvs -v | ");
  while (<MODULES>)
  {
    if (/Concurrent Versions System/)
    {
      $foundit = 1;
    }
  }
  close(MODULES);

  return $foundit;

}

sub RepositoryExists
{
  my($repository) = @_;
  my($existsif);

  $existsif = 0;
  open(MODULES,"cvs -d $repository co -s | ");
  while (<MODULES>)
  {
    if (/Cactus/)
    {
      $existsif = 1;
      last;
    }
  }
  close(MODULES);

  return $existsif;

}
