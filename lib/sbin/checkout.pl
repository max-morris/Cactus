# /usr/bin/perl -s

$sbin_dir = "lib/sbin";

require "$sbin_dir/MakeUtils.pl";

print "\n";

print "Checkout packages or thorns? [packages] : ";
    
$which = <STDIN>;

if ($which =~ /^t/i)
{
    &get_thorns();
}
else
{ 
    &get_packages();
}

print "\nQuit or checkout more packages or thorns [quit] : ";
$dowhat = <STDIN>;
print $dowhat;
if ($dowhat !~ /^t/i && $dowhat !~/^p/i)
{
    print "All done!\n";
    exit;
}

$doit = 1;
while ()
{
    
    if ($dowhat =~ /^t/i)
    {
	&get_thorns();
    }
    else
    { 
	&get_packages();
    }

    print "\nQuit or checkout more packages or thorns [quit] : ";

    $dowhat = <STDIN>;
    if ($dowhat !~ /^t/i && $dowhat !~/^p/i)
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
    
    $range = <STDIN>;
    
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
	    system("(cd packages; cvs checkout $name{$i})");
	}
	
    }
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
    
    $range = <STDIN>;
    
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
	    system("(cd ./packages; cvs -q checkout $name{$i}; cd ..)");
	}
	
    }
}


