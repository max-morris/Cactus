#! /usr/bin/perl 
#/*@@
#  @file      cpp.pl
#  @date      Wed Sep 15 14:21:53 1999
#  @author    Tom Goodale
#  @desc 
#  Replacement C pre-processor
#  @enddesc 
#  @version $Header$
#@@*/

%defines = ();
@filelist = ();
@linelist = ();
@dirlist = ();
$current_wdir = ".";
$cxx = 0;

($source_file, $output_file, $do_deps, @include_path) = &ParseCommandLine(@ARGV);

if($do_deps)
{
  print "$source_file.o $source_file.d :";
}

&ProcessFile($source_file, $output_file, $do_deps, @include_path);

print "\n";

exit;

#/*@@
#  @routine    ParseCommandLine
#  @date       Wed Sep 15 14:22:28 1999
#  @author     Tom Goodale
#  @desc 
#  Checks the command line for options  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub ParseCommandLine
{
  my(@args) = @_;
  my($source_file, $output_file, $do_deps, @include_path) = (0,0,0,("."));

  while($arg = shift(@args))
  {
    if($arg =~ m:^-I(.*):)
    {
      push(@include_path, $1);
    }
    elsif($arg =~ m:^-D([^=]+)(=)?(.*):)
    {
      &define($1, $3);
    }
    elsif($arg =~ m:^-M(.*):)
    {
      $do_deps = 1;
    }
    elsif($arg =~ m:^-C\+\+:)
    {
      $cxx = 1;
    }
    elsif($arg =~ m:^-:)
    {
      die("Unknown preprocessor option '$arg'");
    }
    elsif($source_file)
    {
      die("Source file already set");
    }
    else
    {
      $arg =~ m:(.*/)?(.*):;

      $source_file = $2;
      $current_wdir = $1 if($1);
    }
  }
  
  return ($source_file, $output_file, $do_deps, @include_path);
}


#/*@@
#  @routine    define
#  @date       Wed Sep 15 14:22:58 1999
#  @author     Tom Goodale
#  @desc 
#  Defines a macro
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub define
{
  local($what, $value);

  if($defines{$what})
  {
    print STDERR "Redefining $defines{$what}\n";
  }

  $defines{$what} = $value;
}

#/*@@
#  @routine    ProcessFile
#  @date       Wed Sep 15 14:23:16 1999
#  @author     Tom Goodale
#  @desc 
#  Opens a file and calls the routine to process it.  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub ProcessFile
{
  my($source_file, $output_file, $do_deps, @include_path) = @_;
  my($line, $found, $path);
  local(*F);

  $found = "";

  for $path ($current_wdir, @include_path)
  {
    if (-r "$path/$source_file")
    {
      $found = $path;
      last;
    }
  }
  if($found eq "")
  {
    &Warning("Error: Cannot find $source_file in " . join(":", @include_path));
    exit 2;
  }

  $path = $found;

  $path =~ s:/*$::;

  push(@dirlist, $current_wdir);

  "$path/$source_file" =~ m:(.*/)?(.*):;

  $current_wdir = $1;

  open(F, "$path/$source_file") || &Warning("Error: Unable to open $path/$source_file") || exit 2;

  push(@filelist, "$path/$source_file");


  if($do_deps)
  {
    printf("\\\n      %-40s","$path/$source_file");
  }


  &ParseFile(*F, $output_file, $do_deps, @include_path);
  close(F);

  pop(@filelist);
  $current_wdir = pop(@dirlist);

}

#/*@@
#  @routine    ParseFile
#  @date       Wed Sep 15 14:24:11 1999
#  @author     Tom Goodale
#  @desc 
#  The meat of the program - this actually tries to make sense of the input file  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub ParseFile
{
  local(*F, $output_file, $do_deps, @include_path) = @_;
    
  while($line = <F>)
  {
    chomp($line);

    # Strip off C++ comments
    $line =~ s://.*:: if($cxx); 

    # Strip off C comments
    if($line =~ m:/\*:)
    {
      $line = &StripComments(*F, $line);
    }     

    if($line =~ m:^\#:)
    {
      if($line =~ m:\#include\s+<:)
      {
	# Ignore standard includes
      }
      elsif($line =~ m:^\#include\s+\"([^\"]+)\":)
      {
	push(@linelist, $.);
	&ProcessFile($1, $output_file, $do_deps, @include_path);
	pop(@linelist);
      }
      elsif($line =~ m:^\#define:)
      {
	&CreateDefine(*F, $line);
      }
      elsif($line =~ m:^\#ifdef\s+(.+):)
      {
	if(! &defined($1))
	{
	  if(&FindMatchingEndifOrElse(*F, $line))
	  {
	    # Found an else, so go down a level
	    &ParseFile(*F, $output_file, $do_deps, @include_path);
	  }
	}
	else
	{
	  # Go down a level to process this block.
	  &ParseFile(*F, $output_file, $do_deps, @include_path);
	}
      }
      elsif($line =~ m:^\#ifndef\s+(.+):)
      {
	if(&defined($1))
	{
	  if(&FindMatchingEndifOrElse(*F, $line))
	  {
	    # Found an else, so go down a level
	    &ParseFile(*F, $output_file, $do_deps, @include_path);
	  }
	}
	else
	{
	  # Go down a level to process this block.
	  &ParseFile(*F, $output_file, $do_deps, @include_path);
	}
      }
      elsif($line =~ m:^\#if\s+(.+):)
      {
	if(! &IfLine($1))
	{
	  if(&FindMatchingEndifOrElse(*F, $line))
	  {
	    # Found an else, so go down a level
	    &ParseFile(*F, $output_file, $do_deps, @include_path);
	  }
	}
	else
	{
	  # Go down a level to process this block.
	  &ParseFile(*F, $output_file, $do_deps, @include_path);
	}
      }
      elsif($line =~ m:^\#else:)
      {
	if(&FindMatchingEndifOrElse(*F, $line))
	{
	  &Warning("Error: Syntax error - only one else allowed.");
	  exit 2;
	}
	# Return to previous level at an endif
	return;
      }
      elsif($line =~ m:^\#endif:)
      {
	# Return to previous level at an endif
	return;
      }
      elsif($line =~ m:^\#undef:)
      {
	&UnDefine($line);
      }
      else
      {
	&Warning("Warning: Unknown preprocessor directive $line");
      }
    }
    else
    {
      if(! $do_deps)
      {
	$line = &ExpandLine($line);
	
	print "$line\n";
      }
    }
  }

  return;
}

#/*@@
#  @routine    CreateDefine
#  @date       Wed Sep 15 14:24:51 1999
#  @author     Tom Goodale
#  @desc 
#  Takes a line with #define on it and creates the appropriate macro
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub CreateDefine
{
  local(*F, $line) = @_;
  my($var, $val);

  $line =~ m:\#define\s+([^\s]+)\s*(.*):;
  
  $var = $1;
  $val = $2;

  if($val =~ m:\\$:)
  {
    while($val =~ m:\\$:)
    {
      $val =~ s:\\$::;
      $line = <F>;
      chomp($line);

      $val .= $line;
    }
  }

  if($val eq "")
  {
    $val = "__cctk__internal_val__%%%%";
  }
  $defines{$var} = $val;
  
}


#/*@@
#  @routine    UnDefine
#  @date       Wed Sep 15 14:25:27 1999
#  @author     Tom Goodale
#  @desc 
#  Undefines a macro given a line with #undef on it.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub UnDefine
{
  my($line) = @_;
  my($var);

  $line =~ m:\#undef\s+([^\s]+):;
  
  $var = $1;

  delete $defines{$var};
  
}


#/*@@
#  @routine    defined
#  @date       Wed Sep 15 14:25:55 1999
#  @author     Tom Goodale
#  @desc 
#  Checks to see if a macros is defined.  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub defined
{
  my($var) = @_;

  return 1 if defined $defines{$var};
  
  return 0;
}


#/*@@
#  @routine    FindMatchingEndifOrElse
#  @date       Wed Sep 15 14:26:23 1999
#  @author     Tom Goodale
#  @desc 
#  Skips through a file until there is an else or an endif without a matching if.
#  Return 1 if it is an else, 0 if it is an endif.  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub FindMatchingEndifOrElse
{
  local(*F, $line) = @_;
  my($if_count,$retval) = (0,0);

  while(<F>)
  {
    if(m:^\#ifdef:)
    {
      $if_count++;
      next;
    }
    elsif(m:\#endif:)
    {
       last if($ifcount == 0);
 
      $if_count--;
    }
    elsif(m:\#else:)
    {
      if($ifcount == 0)
      {
	$retval = 1;
	last;
      }
	  
      $if_count--;
    }
  }

  return $retval;
}

#/*@@
#  @routine    ExpandLine
#  @date       Wed Sep 15 14:27:18 1999
#  @author     Tom Goodale
#  @desc 
#  Expands all macros in a line.  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub ExpandLine
{
  my($line) = @_;
  my(@tokens);
  my($stringify);

  @tokens = split(/(\W+)/, $line);

  $line = "";

  $stringify = 0;
  for $token (@tokens)
  {
    if($token =~ m:\w:&& $stringify == 0 && defined $defines{$token})
    {
      $line .= &ExpandLine($defined{$token});
    }
    elsif($stringify == 1)
    {
      $line .= "\"$token\"";
      $stringify = 0;
    }
    else
    {
      # Concatanation character
      $token =~ s:\s*\#\#\s*::;

      # Check for stringification.
      if($token =~ m:\#$:)
      {
	$stringify = 1;
	$token =~ s:\#$::;
      }
      $line .= $token;
    }
  }

  return $line;
}

#/*@@
#  @routine    StripComments
#  @date       Wed Sep 15 14:27:49 1999
#  @author     Tom Goodale
#  @desc 
#  Strips out C comments  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub StripComments
{
  local(*F, $line) = @_;
  my($var, $val);
  

  if($line !~ m:\*/:)
  {
    while($line !~ m:\*/:)
    {
      $line .= <F>;
      chomp($line);
    }
  }

  # Strip off comments
  if($line =~ m:\*/:)
  {
    $line =~ s:/\*.*\*/::g;
    return $line;
  }

  return $line;
}


#/*@@
#  @routine    Warning
#  @date       Wed Sep 15 14:28:11 1999
#  @author     Tom Goodale
#  @desc 
#  Prints a warning to stderr.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub Warning
{
  my($message) = @_;
  my($number);
  
  print STDERR "$message at " . $filelist[$#filelist] . " line $.\n";

  for($number = $#filelist -1; $number >= 0; $number--)
  {
    print STDERR "   opened from $filelist[$number] line $linelist[$number]\n";
  }

  return 0;
}

#/*@@
#  @routine    IfLine
#  @date       Wed Sep 15 14:57:22 1999
#  @author     Tom Goodale
#  @desc 
#  Parses a #if line  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub IfLine
{
  my($statements) = @_;
  my(@tokens);
  my($toknum);
  my($state);

  @tokens = split(/(\W+)/, $statements);

  $state = 0;
  $toknum = 0;
  $lastop = "||";

  while($toknum <= $#tokens)
  {
    if($tokens[$toknum] =~ m:^defined$:)
    {
      $currstate = &defined($tokens[$toknum+2]);

      eval "$state = $state $lastop $currstate";
      $toknum +=4
    }
    elsif($tokens[$toknum] =~ m:\|\||\&\&:)
    {
      $lastop = $token[$toknum];
      $token++;
    }
    else
    {
      $currstate = &defined($tokens[$toknum]);

      eval "$state = $state $lastop $currstate";
      $toknum ++;
    }
      
  }

  return $state;
}


       
