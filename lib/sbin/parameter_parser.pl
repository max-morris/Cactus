#! /usr/bin/perl

#%implementations = ("flesh", "flesh", "test1", "test1", "test2", "test2");

#%parameter_database = create_parameter_database(%implementations);

#&print_parameter_database(%parameter_database);

#/*@@
#  @routine    create_parameter_database
#  @date       Wed Sep 16 11:45:18 1998
#  @author     Tom Goodale
#  @desc 
#  Creates a database of all the parameters
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_parameter_database
{
  local(%thorns) = @_;
  local($thorn, @indata);
  local(@new_parameter_data);
  local(@parameter_data);
  
  #  Loop through each implementation's parameter file.
  foreach $thorn (keys %thorns)
  {
    #       Read the data
    @indata = &read_file("$thorns{$thorn}/param.ccl");
    
    #       Get the parameters from it
    @new_parameter_data = &parse_param_ccl($thorn, @indata);
    
    #       Add the parameters to the master parameter database
    push (@parameter_data, @new_parameter_data);
    
  }
  
  @parameter_data = &cross_index_parameters(scalar(keys %thorns), (keys %thorns), @parameter_data);
  
  return @parameter_data;
}

sub cross_index_parameters
{
  local($n_thorns, @indata) = @_;
  local(@thorns);
  local(%parameter_database);
  local(@module_file);
  local($line);
  local(@data);
  local($thorn);
  
  @thorns = @indata[0..$n_thorns-1];
  %parameter_database = @indata[$n_thorns..$#indata];
  
  $parameter_database{"GLOBAL PARAMETERS"} = "";
  
  foreach $thorn (@thorns)
  {
    foreach $parameter (split(/ /, $parameter_database{"\U$thorn\E GLOBAL variables"}))
    {
      if($public_parameters{"\U$parameter\E"})
      {
	  $message = "Duplicate public parameter $parameter, defined in $imp and ".$public_parameters{"\Uparameter\E"};
	  &CST_error(0,$message,__LINE__,__FILE__);
      }
      else
      {
	$public_parameters{"\Uparameter\E"} = "$thorn";
	
	$parameter_database{"GLOBAL PARAMETERS"} .= "$thorn\::$parameter ";
      }
    }
  }
  
  return %parameter_database;
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
# 
#  @endhistory 
#@@*/

sub read_file
{
  local($file) = @_;
  local(@indata);
  
  open(IN, "<$file") || die("Can't open $file\n");
  
  while(<IN>)
  {
    $_ =~ s/\#.*//;
    
    next if(m/^\s+$/);
    
    &chompme($_);
    
    push(@indata, $_);
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
    local($in) = @_;

    $lastchar = chop($in);
    if ($lastchar == "\n")
    {
	return $_;
    }
    else
    {
	return $in;
    }
}

#/*@@
#  @routine    parse_param_ccl
#  @date       Wed Sep 16 11:55:33 1998
#  @author     Tom Goodale
#  @desc 
#  Parses a param.ccl file and generates a database of the values.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub parse_param_ccl
{
  local($thorn, @data) = @_;
  local($line_number, $line, $block, $type, $variable, $description);
  local($current_friend, $new_ranges, $new_desc);
  local($data, %parameter_db);
  local(%friends);
  local(%defined_parameters);
  
  
  #   The default block is private.
  $block = "PRIVATE";
  
  # Initialise, to prevent perl -w from complaining.
  $parameter_db{"\U$thorn PRIVATE\E variables"} = "";

  for($line_number = 0; $line_number < @data; $line_number++)
  {
    $line = $data[$line_number];
    
    #       Parse the line
    if($line =~ m/(GLOBAL|RESTRICTED|PRIVATE|SHARES)\s*:(.*)/i)
    {
      #           It's a new block.
      $block = "\U$1\E";
      
      if($block eq "SHARES")
      {
	$current_friend = $2;
	$current_friend =~ s:\s::;
	
	#               It's a friend block.
	$block .= " \U$current_friend\E";
	#               Remember this friend, but make the memory unique.
	$friends{"\U$current_friend\E"} = 1;
      }
      
      # Do some initialisation to prevent perl -w from complaining.
      if(!$parameter_db{"\U$thorn $block\E variables"})
      {
	$parameter_db{"\U$thorn $block\E variables"} = "";
      }
    }
    elsif($line =~ m:(EXTENDS |USES )?\s*(?\:CCTK_)?(INT|REAL|LOGICAL|KEYWORD|STRING)\s*([a-zA-Z]+[a-zA-Z0-9_]*) \s*(\"[^\"]*\"):i)
    {

      # This is a parameter definition.
      $type = "\U$2\E";
      $variable = $3;
      $description = $4;
      
      if($defined_parameters{"\U$variable\E"})
      {

	$message = "Duplicate parameter $variable in thorn $thorn. Ignoring second definition";
	&CST_error(1,$message,__LINE__,__FILE__);

	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      elsif($1 && $1 =~ m:(EXTENDS|USES):i && $block !~ m:SHARES\s*\S:)
      {
	# Can only extend a friend variable.
	$message =  "Parse error in $thorn/param.ccl";
	&CST_error(0,$message,__LINE__,__FILE__);
	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      elsif($data[$line_number+1] !~ m:^\s*\{\s*$:)

      {
	# Since the data should have no blank lines, the next
	# line should have { on it.
	$message = "Parse error in $thorn/param.ccl - missing \"{\" in definition of parameter \"$variable\"";
	&CST_error(0,$message,__LINE__,__FILE__);
	# Move past the end of this block.
	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      else
      {
	# Move past {
	$line_number++;
	$line_number++;
	
	# Store data about this variable.
	$defined_parameters{"\U$variable\E"} = 1;
	
	$parameter_db{"\U$thorn $block\E variables"} .= $variable." ";
	$parameter_db{"\U$thorn $variable\E type"} = $type;
	$parameter_db{"\U$thorn $variable\E description"} = $description;
	$parameter_db{"\U$thorn $variable\E ranges"} = 0;
	
	# Parse the allowed values and their descriptions.
        # The (optional) description is seperated by ::
	while($data[$line_number] !~ m:\s*\}:)
	{
	    ($new_ranges, $delim, $new_desc) = $data[$line_number] =~ m/(.*)(::)(.*)/;
	    # Increment the number of ranges found (ranges)
	    $parameter_db{"\U$thorn $variable\E ranges"}++;
	    # Strip out any spaces in the range for a numeric parameter.
	    if($type =~ m:INT|REAL:)
	    {
		$new_ranges =~ s/[ \t]+/ /g;
	    }
	    $parameter_db{"\U$thorn $variable\E range $parameter_db{\"\U$thorn $variable\E ranges\"} range"} = $new_ranges;
	    # Give a warning if no description has been given
	    if(! $delim)
	    {
		$message = "Missing description of range '$new_ranges' for parameter $thorn\::$variable";
		&CST_error(1,$message,__LINE__,__FILE__);
	    }
	    $parameter_db{"\U$thorn $variable\E range $parameter_db{\"\U$thorn $variable\E ranges\"} description"} = $new_desc;
	    $line_number++;
	}

        # Give a warning if no range was given and it was needed
        if ($parameter_db{"\U$thorn $variable\E ranges"}==0 && $type =~ m:INT|REAL:)
        {
	    $message = "No range given for $variable in $thorn";
	    &CST_error(0,$message,__LINE__,__FILE__);
        }
	if($block !~ m:SHARES:)
	{
	  if($data[$line_number] =~ m:\s*\}\s*(.+):)
	  {
	      $default = $1;
	      if ($type =~ m:INT|REAL: && $default =~ m:":)
	      {
		  $message = "String default given for $type $variable in $thorn";
                  &CST_error(0,$message,__LINE__,__FILE__);
	      }
              elsif ($type =~ m:STRING|KEYWORD: && $default !~ m:".*":)
              {
		  $message = "Default given for $type $variable in $thorn is not a string";
                  &CST_error(0,$message,__LINE__,__FILE__);
	      }
              elsif ($type =~ m:LOGICAL: && $default =~ m:": && $default !~ m:".*":)
	      {
		  $message = "Default given for $type $variable in $thorn is missing a quote";
                  &CST_error(0,$message,__LINE__,__FILE__);
	      }

	      $parameter_db{"\U$thorn $variable\E default"} = $default;
	  }
	  else
	  {
	    $message =  "Unable to find default for $variable";
	    &CST_error(0,$message,__LINE,__FILE);
	  }		
	}
      }
    }
    else
    {
      if($line =~ m:\{:)
      {
	$message = "Skipping parameter block in $thorn with missing keyword";
        &CST_error(1,$message,__LINE__,__FILE__);
	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      else
      {
	$message = "Unknown line \"$line\" in $thorn/param.ccl";
        &CST_error(0,$message,__LINE__,__FILE__);
      }
    }
  }
  
  $parameter_db{"\U$thorn\E SHARES implementations"} = join(" ", keys %friends);
  
  return %parameter_db;
}

#/*@@
#  @routine    print_parameter_database
#  @date       Wed Sep 16 14:58:52 1998
#  @author     Tom Goodale
#  @desc 
#  Prints out a parameter database.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/
sub print_parameter_database
{
  local(%parameter_database) = @_;
  local($field);
  
  foreach $field ( sort keys %parameter_database )
  {
    print "$field has value $parameter_database{$field}\n";
  }
}


1;


