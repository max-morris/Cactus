
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
    my($level,$mess,$line,$file) = @_;

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



#/*@@
#  @routine    read_file
#  @date       Wed Sep 16 11:54:38 1998
#  @author     Tom Goodale
#  @desc 
#  Reads a file deleting comments and blank lines. 
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#  @hdate Fri Sep 10 10:25:47 1999 @hauthor Tom Goodale
#  @hdesc Allows a \ to escape the end of a line. 
#  @endhistory 
#@@*/

sub read_file
{
  my($file) = @_;
  my(@indata);
  my($line);

  open(IN, "<$file") || die("Can't open $file\n");
  
  $line = "";

  while(<IN>)
  {
    $_ =~ s/\#.*//;
    
    next if(m/^\s+$/);
 
    &chompme($_);

    # Add to the currently processed line.
    $line .= $_;

    # Check the line for line-continuation
    if(m:[^\\]\\$:)
    {
      $line =~ s:\\$::;
    }
    else
    {
      push(@indata, $line);
      $line = "";
    }
  }
  
  # Make sure to dump out the last line, even if it ends in a \
  if($line ne "")
  {
    push(@indata, $line);
  }
  close IN;
  
  return @indata;
}


#/*@@
#  @routine    chompme
#  @date       Mon 26th April 1999
#  @author     Gabrielle Allen
#  @desc 
#  Implements a version of the perl5 chomp function,
#  returning the string passed in with the last character
#  removed unless it is a newline
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub chompme
{
    my($in) = @_;

    $lastchar = chop($in);
    if ($lastchar eq "\n")
    {
	return $_;
    }
    else
    {
	return $in;
    }
}

#/*@@
#  @routine    WriteFile
#  @date       Tue Oct 19 21:09:12 CEST 1999 
#  @author     Gabrielle Allen
#  @desc 
#  Writes a file only if the contents haven't changed
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub WriteFile
{
  my ($filename,$rdata) = @_;
  my ($data_in);

# Read in file
  $data_in = "";
  if (-e $filename) 
  {
    open(IN, "<$filename");
    while (<IN>)
    {
      $data_in .= $_;
    }
  }

  if ($$rdata ne $data_in)   
  {
#    print "Creating new file $filename\n";
    open(OUT, ">$filename") || die("Can't open $filename\n");
    print OUT $$rdata;
    close OUT;
  }

}

1;

