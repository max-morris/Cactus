

#/*@@
#  @routine   buildthorns
#  @date      Tue Jan 19 14:02:07 1999
#  @author    Tom Goodale
#  @desc 
#  Creates an ActiveThornsList
#  @enddesc 
#  @version $Id$
#@@*/

sub buildthorns
{
    local($package_dir,$choice) = @_;

    chdir $package_dir || die "Can't change directory to $package_dir";

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
	    push (@packages, $_);
	}
    }
    
    close PACKAGES;

    if ($choice =~ "thorns")
    {
    
	foreach $package (@packages)
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
	@total_list = @packages;
    }

    foreach $thorn (@total_list)
    {
	print "$thorn\n";
    }
}

1;
