#/*@@
#  @file      CSTUtils.pl
#  @date      4 July 1999
#  @author    Gabrielle Allen
#  @desc 
#  Various utility routines.
#  @enddesc
#  @version $Header$ 
#@@*/

#/*@@
#  @routine   CST_error
#  @date      4 July 1999
#  @author    Gabrielle Allen
#  @desc 
#  Print an error or warning message
#  @enddesc 
#@@*/

sub CST_error
{
    my($level,$mess,$help,$line,$file) = @_;
    my($error);

    if ($help !~ /^\s*$/)
    {
      $help = "     HINT: $help\n";
    }

    if ($full_warnings)
    {
        if ($level == 0)
        {
            $CST_errors++;
            $error = "\nCST error in $file (at $line)\n  -> $mess\n";
            print STDERR "$error\n";
            $error_string .= "$error$help\n";
        }
        else
        {
            $error = "\nCST warning in $file (at $line)\n  -> $mess\n";
            print STDERR "$error\n";
            $error_string .= "$error$help\n";
        }
    }
    else
    {
        if ($level == 0)
        {
            $CST_errors++;
            $error = "\nCST error $CST_errors:\n  -> $mess\n";
            print STDERR "$error\n";
            $error_string .= "$error$help\n";
        }
        else
        {
            $error = "\nCST warning:\n  -> $mess\n";
            print STDERR "$error\n";
            $error_string .= "$error$help\n";
        }           
    }

    return;
}


#/*@@
#  @routine   CST_PrintErrors
#  @date      5 December 1999
#  @author    Gabrielle Allen
#  @desc 
#  Print all the errors and warnings from the CST
#  @enddesc 
#  @version $Id$
#@@*/

sub CST_PrintErrors
{
  if($error_string)
  {
    print "\n\n------------------------------------------------------\n";
    print "Warnings were generated during execution of the CST\n";
    print "------------------------------------------------------\n\n";
    print "$error_string";
    print "------------------------------------------------------\n\n";
  }
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
    chomp;

    # Remove comments.
    $_ = &RemoveComments($_);
#    $_ =~ s/\#.*//;
    
    # Ignore empty lines.
    next if(m/^\s*$/);
 
    #&chompme($_);

    # Add to the currently processed line.
    $line .= $_;

    # Check the line for line-continuation
    if(m:[^\\]\\\s*$:)
    {
      $line =~ s:\\\s*$::;
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

# Strip any matching quotes from filename
  $filename =~ s/^\s*\"(.*)\"\s*$/\1/;
  $filename =~ s/^\s*\'(.*)\'\s*$/\1/;

# Read in file
  $data_in = "";
  if (-e $filename) 
  {
    open(IN, "< $filename");
    $data_in = join ('', <IN>);
    close IN;
  }

  if ($$rdata ne $data_in)   
  {
#    print "Creating new file $filename\n";
    open(OUT, ">$filename") || die("Can't open $filename\n");
    print OUT $$rdata;
    close OUT;
  }

}

#/*@@
#  @routine    TestName
#  @date       Sat Dec 16 1.48
#  @author     Gabrielle Allen
#  @desc 
#  Check thorn/arrangement name is valid
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub TestName
{
  local($thorn,$name) = @_;
  local($valid);

  $valid = 1;

  if (!$name)
  {
    $valid = 0;
  }
  elsif ($name !~ /^[a-zA-Z]/)
  {
    print STDERR "Name must begin with a letter!\n\n";
    $valid = 0;
  }
  elsif ($name !~ /^[a-zA-Z0-9_]*$/)
  {
    print STDERR "Name can only contain letters, numbers or underscores!\n\n";
    $valid = 0;
  }

  if ($thorn && length($name)>27)
  {
    print STDERR "Thorn names must be 27 characters or less!\n\n";
    $valid = 0;
  }

  if ($thorn && $name eq "doc")
  {
    print STDERR "Thorn name doc is not allowed!\n\n";
    $valid = 0;
  }

  return $valid;
}

#/*@@
#  @routine    SplitWithStrings
#  @date       Tue May 21 23:45:54 2002
#  @author     Tom Goodale
#  @desc 
#  Splits a string on spaces and = ignoring
#  any occurence of these in strings.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#  @var     expression
#  @vdesc   Expression to split
#  @vtype   string
#  @vio     in
#  @endvar 
#
#  @returntype list
#  @returndesc
#    Split representation of input expression.
#  @endreturndesc
#@@*/
sub SplitWithStrings
{
  my ($expression, $thorn) = @_;

  my $insstring = 0;
  my $indstring = 0;
  my $escaping = 0;

  my @tokens = ();

  my $token="";

  # First split the string into string tokens and split tokens we are
  # allowed to split.

  for $i (split(//,$expression))
  {
    if($i eq '\\')
    {
      if($escaping)
      {
        $token .= $i;
      }
      
      $escaping = 1 - $escaping;
    }
    elsif($i eq '"' && ! $insstring && ! $escaping)
    {
      if(length $token > 0 || $indstring)
      {
        push(@tokens, $token);
      }

      $token = "";
      $indstring = 1 - $indstring;
    }
    elsif($i eq "'" && ! $indstring && ! $escaping)
    {
      if(length $token > 0 || $insstring)
      {
        push(@tokens, $token);
      }

      $token = "";

      $insstring = 1 - $insstring;
    }
    elsif($i =~ /^\s+$/ && ! $insstring && ! $indstring && ! $escaping)
    {
      if(length $token > 0 || $insstring)
      {
        push(@tokens, $token);
      }

      $token = "";
    }
    elsif($i eq '=' && ! $insstring && ! $indstring && ! $escaping)
    {
      if(length $token > 0 || $insstring)
      {
        push(@tokens, $token);
      }

      $token = "";
    }
    else
    {
      if($escaping)
      {
        $token .= "\\";
        $escaping = 0; 
      }
      $token .= "$i";
    }
  }

  if($insstring || $indstring)
  {
    print "Error: Unterminated string while parsing interface for thorn : $thorn\n"
  }

  if($escaping)
  {
    $token .= '\\';
  }
  
  if(length $token > 0)
  {
    push(@tokens, $token);
  }
  
  return @tokens;

}

#/*@@
#  @routine    RemoveComments
#  @date       
#  @author     Tom Goodale, Yaakoub El Khamra
#  @desc 
#  Removes comments from lines
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#  @var     line
#  @vdesc   line to remove comments from
#  @vtype   string
#  @vio     in
#  @endvar 
#
#  @returntype line
#  @returndesc
#    line without comments
#  @endreturndesc
#@@*/
sub RemoveComments
{
  my ($line) = @_;
  my $nocomment = $line;
  my $insstring = 0;
  my $indstring = 0;
  my $escaping = 0;
  my $token="";

  for $i (split(//,$line))
  {

    if($i eq '\\')
    {
      if($escaping)
      {
        $token .= $i;
      }
      
      $escaping = 1 - $escaping;
    }
    elsif($i eq '"' && ! $insstring && ! $escaping)
    {
      $token = "";
      $indstring = 1 - $indstring;
    }
    elsif($i eq "'" && ! $indstring && ! $escaping)
    {
      $token = "";
      $insstring = 1 - $insstring;
    }
    elsif($i =~ /^\s+$/ && ! $insstring && ! $indstring && ! $escaping)
    {
      $token = "";
    }
    elsif($i eq '=' && ! $insstring && ! $indstring && ! $escaping)
    {
      $token = "";
    }
    elsif($i eq '#' && ! $insstring && ! $indstring && ! $escaping)
    {
      $nocomment =~ s/\#.*//;
      return $nocomment;
    }
    else
    {
      if($escaping)
      {
        $token .= "\\";
        $escaping = 0; 
      }
      $token .= "$i";
    }
  }

  if($insstring || $indstring)
  {
    print "Error: Unterminated string while parsing interface for thorn : $thorn\n";
    print $nocomment;
  }

  if($escaping)
  {
    $token .= '\\';
  }
  
  return $nocomment;
}

1;
