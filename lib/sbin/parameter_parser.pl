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
  
  $parameter_database{"PUBLIC PARAMETERS"} = "";
  
  foreach $thorn (@thorns)
  {
    foreach $parameter (split(/ /, $parameter_database{"\U$thorn\E PUBLIC variables"}))
    {
      if($public_parameters{"\U$parameter\E"})
      {
	print STDERR "Duplicate public parameter $parameter\n";
	print STDERR "Parameter defined in $imp and in " . 
	  $public_parameters{"\Uparameter\E"};
	die("****Fatal error***");
      }
      else
      {
	$public_parameters{"\Uparameter\E"} = "$thorn";
	
	$parameter_database{"PUBLIC PARAMETERS"} .= "$thorn\::$parameter ";
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
    
    chomp;
    
    push(@indata, $_);
  }
  
  close IN;
  
  return @indata;
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
    if($line =~ m/(PUBLIC|PROTECTED|PRIVATE|FRIEND)\s*:(.*)/i)
    {
      #           It's a new block.
      $block = "\U$1\E";
      
      if($block eq "FRIEND")
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
    elsif($line =~ m:(EXTENDS )?\s*(INTEGER|REAL|LOGICAL|KEYWORD|STRING)\s*([a-zA-Z]+[a-zA-Z0-9_]*) \s*(\"[^\"]*\"):i)
    {

      # This is a parameter definition.
      $type = "\U$2\E";
      $variable = $3;
      $description = $4;
      
      if($defined_parameters{"\U$variable\E"})
      {
	print STDERR "Duplicate parameter $variable in thorn $thorn\n";
	print STDERR "Ignoring second definition.\n";
	$CST_errors++;
	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      elsif($1 && $1 =~ m:EXTENDS:i && $block !~ m:FRIEND\s*\S:)
      {
	# Can only extend a friend variable.
	print STDERR "Parse error at line $line_number\n";
	$CST_errors++;
	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      elsif(! $data[$line_number+1] =~ m:^\s*\{\s*$:)
      {
	# Since the data should have no blank lines, the next
	# line should have { on it.
	print STDERR "Parse error at line $line_number\n";
	$CST_errors++;
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
	  if($type =~ m:INTEGER|REAL:)
	  {
	    $new_ranges =~ s/[ \t]+/ /g;
	  }
	  $parameter_db{"\U$thorn $variable\E range $parameter_db{\"\U$thorn $variable\E ranges\"} range"} = $new_ranges;
	  # Give a warning if no description has been given
	  if(! $delim)
	  {
	    print STDERR "Missing description of range '$new_ranges' for parameter $thorn\::$variable\n";
	  }
	  $parameter_db{"\U$thorn $variable\E range $parameter_db{\"\U$thorn $variable\E ranges\"} description"} = $new_desc;
	  $line_number++;
	}
        # Give a warning if no range was given and it was needed
        if ($parameter_db{"\U$thorn $variable\E ranges"}==0 && $type =~ m:INTEGER|REAL:)
        {
	    print STDERR "No range given for $variable in $thorn\n";
            $CST_errors++;
        }
	if($block !~ m:FRIEND:)
	{
	  if($data[$line_number] =~ m:\s*\}\s*(.+):)
	  {
	      $default = $1;
#	      print "type is $type, default is $default\n";
	      if ($type =~ m:INTEGER|REAL: && $default =~ m:":)
	      {
		  print STDERR "String default given for $type $variable in $thorn\n";
		  $CST_errors++;
	      }
	      $parameter_db{"\U$thorn $variable\E default"} = $default;
	  }
	  else
	  {
	    print STDERR "Unable to find default for $variable\n";
	    $CST_errors++;
	  }		
	}
      }
    }
    else
    {
      if($line =~ m:\{:)
      {
	print STDERR "...Skipping parameter block with missing keyword....\n";
	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      else
      {
	print STDERR "Unknown line $line!!!\n";
      }
    }
  }
  
  $parameter_db{"\U$thorn\E FRIEND implementations"} = join(" ", keys %friends);
  
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
