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
    local(%implementations) = @_;
    local($imp, @indata);
    local(@new_parameter_data);
    local(@parameter_data);

#  Loop through each implementation's parameter file.
    foreach $imp (keys %implementations)
    {
#       Read the data
	@indata = &read_file("$implementations{$imp}/param.ccl");
	
#       Get the parameters from it
	@new_parameter_data = &parse_param_ccl($imp, @indata);

#       Add the parameters to the master parameter database
	push (@parameter_data, @new_parameter_data);

    }

    @parameter_data = &cross_index_parameters(scalar(keys %implementations), (keys %implementations), @parameter_data);

    return @parameter_data;
}

sub cross_index_parameters
{
  local($n_implementations, @indata) = @_;
  local(@implementations);
  local(%parameter_database);
  local(@module_file);
  local($line);
  local(@data);

  @implementations = @indata[0..$n_implementations-1];
  %parameter_database = @indata[$n_implementations..$#indata];

  foreach $imp (@implementations)
  {
    foreach $parameter (split(/ /, $parameter_database{"\U$imp\E PUBLIC variables"}))
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
	$public_parameters{"\Uparameter\E"} = "$imp";
	
	$parameter_database{"PUBLIC PARAMETERS"} .= "$imp\::$parameter ";
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
	
	chop;
	
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
    local($implementation, @data) = @_;
    local($linenum, $line, $block, $type, $variable, $description, $nerrors);
    local($current_friend, $new_ranges, $new_desc);
    local($data, %parameter_db);
    local(%friends);
    local(%defined_parameters);


#   The default block is private.
    $block = "PRIVATE";

    for($linenum = 0; $linenum < @data; $linenum++)
    {
	$line = $data[$linenum];

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
	}
	elsif($line =~ m:(EXTENDS )?\s*(INTEGER|REAL|KEYWORD|STRING)\s*([a-zA-Z]+[a-zA-Z0-9_]*) \s*(\"[^\"]*\"):i)
	{

#           This is a parameter definition.
	    $type = "\U$2\E";
	    $variable = $3;
	    $description = $4;

	    if($defined_parameters{"\U$variable\E"})
	    {
	      print STDERR "Duplicate parameter $variable in implementation $implementation\n";
	      print STDERR "Ignoring second definition.\n";
		$nerrors++;
		$linenum++ until ($data[$linenum] =~ m:\}:);
	    }
	    elsif($1 =~ m:EXTENDS:i && $block ne "FRIEND")
	    {
#               Can only extend a friend variable.
		print STDERR "Parse error at line $linenum\n";
		$nerrors++;
		$linenum++ until ($data[$linenum] =~ m:\}:);
	    }
	    elsif(! $data[$linenum+1] =~ m:^\s*\{\s*$:)
	    {
#               Since the data should have no blank lines, the next
#               line should have { on it.
		print STDERR "Parse error at line $linenum\n";
		$nerrors++;
#               Move past the end of this block.
		$linenum++ until ($data[$linenum] =~ m:\}:);
	    }
	    else
	    {
#               Move past {
		$linenum++;
		$linenum++;

#               Store data about this variable.
		$defined_parameters{"\U$variable\E"} = 1;

		$parameter_db{"\U$implementation $block\E variables"} .= $variable." ";
		$parameter_db{"\U$implementation $variable\E type"} = $type;
		$parameter_db{"\U$implementation $variable\E description"} = $description;
		$parameter_db{"\U$implementation $variable\E ranges"} = 0;

#               Parse the allowed values and their descriptions.
		while(($new_ranges, $new_desc) = $data[$linenum] =~ m/(.*)::(.*)/)
		{
		    $parameter_db{"\U$implementation $variable\E ranges"}++;
		    $parameter_db{"\U$implementation $variable\E range $parameter_db{\"\U$implementation $variable\E ranges\"} range"} = $new_ranges;
		    $parameter_db{"\U$implementation $variable\E range $parameter_db{\"\U$implementation $variable\E ranges\"} description"} = $new_desc;
		    $linenum++;
		}
		if($block !~ m:FRIEND:)
		{
		    if($data[$linenum] =~ m:\s*\}\s*(.+):)
		    {
			$parameter_db{"\U$implementation $variable\E default"} = $1;
		    }
		    else
		    {
			print STDERR "Unable to find default for $variable\n";
			$nerrors++;
		    }		
		}
	    }
	}
	else
	{
	    if($line =~ m:\{:)
	    {
		print STDERR "...Skipping block with missing keyword....\n";
		$linenum++ until ($data[$linenum] =~ m:\}:);
	    }
	    else
	    {
		print STDERR "Unknown line $line!!!\n";
	    }
	}
    }

    $parameter_db{"\U$implementation\E FRIEND implementations"} = join(" ", keys %friends);

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

    foreach $field ( sort keys %parameter_database ){
	print "$field has value $parameter_database{$field}\n";
    }
}

1;
