#/*@@
#  @file      MakeUtils.pl
#  @date      July 1999
#  @author    Tom Goodale
#  @desc 
#  Utility perl routines needed by the Makefile.
#  @enddesc 
#  @version $Header$
#@@*/


#/*@@
#  @routine   buildthorns
#  @date      Tue Jan 19 14:02:07 1999
#  @author    Tom Goodale
#  @desc 
#  Creates an compiled ThornList
#  @enddesc 
#  @version $Id$
#@@*/

sub buildthorns
{
    local($package_dir,$choice) = @_;
    local(@arrangements);

    chdir $package_dir || die "Can't change directory to $package_dir\n";

    open(PACKAGES, "ls|");
    
    while(<PACKAGES>)
    {
	chop;

	# Ignore CVS and backup stuff
	next if (m:^CVS$:);
	next if (m:^\#:);
	next if (m:~$:);
	next if (m:\.bak$:i);
	next if (m:^\.:);
	
	# Just pick directories
	if( -d $_)
	{
	    push (@arrangements, $_);
	}
    }
    
    close PACKAGES;

    if ($choice =~ "thorns")
    {
    
	foreach $package (@arrangements)
	{
	    chdir $package;
	    
	    open(THORNLIST, "ls|");
	    
	    while(<THORNLIST>)
	    {
		chop;
		
		# Ignore CVS and backup stuff
		next if (m:^CVS$:);
		next if (m:^\#:);
		next if (m:~$:);
		next if (m:\.bak$:i);
		next if (m:^\.:);
		
		# Allow each package to have a documentation directory.
		next if (m:^doc$:);
		
		# Just pick directories
		if( -d $_)
		{
		    push(@total_list, "$package/$_");
		}
	    }
	    chdir "..";
	}
    
    }
    else
    {
	@total_list = @arrangements;
    }

    foreach $thorn (@total_list)
    {
	print "$thorn\n";
    }

    chdir "..";
}

1;
