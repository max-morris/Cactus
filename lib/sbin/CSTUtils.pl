
#/*@@
#  @routine   CST_error
#  @date      4 July 1999
#  @author    Gabrielle Allen
#  @desc 
#  Print an error or warning message
#  @enddesc 
#  @version $Id$
#@@*/

sub CST_error
{
    local($level,$mess,$line,$file) = @_;

    if ($full_warnings)
    {
	if ($level == 0)
	{
	    $CST_errors++;
	    print STDERR "CST error in $file (at $line)\n";
	    print STDERR "  -> $mess\n";
	}
	else
	{
	    print STDERR "CST warning in $file (at $line)\n";
	    print STDERR "  -> $mess\n";
	}
    }
    else
    {
	if ($level == 0)
	{
	    $CST_errors++;
	    print STDERR "CST error $CST_errors:\n  -> $mess\n";
	}
	else
	{
	    print STDERR "CST warning:\n  -> $mess\n";
	}	    
    }
    return;
}


1;

