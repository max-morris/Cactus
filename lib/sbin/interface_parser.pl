#! /usr/bin/perl

#require "parameter_parser.pl";


#%implementations = ("flesh", "flesh", "test1", "test1", "test2", "test2");

#%interface_database = create_interface_database(%implementations);

#&print_interface_database(%interface_database);

#/*@@
#  @routine    create_interface_database
#  @date       Wed Sep 16 15:07:11 1998
#  @author     Tom Goodale
#  @desc 
#  Creates a database of all the interfaces
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_interface_database
{
  local(%thorns) = @_;
  local($thorn, @indata);
  local(@new_interface_data);
  local(@interface_data);
  
  #  Loop through each  thorn's interface file.
  foreach $thorn (keys %thorns)
  {
    #       Read the data
    @indata = read_file("$thorns{$thorn}/interface.ccl");
    
    #       Get the interface data from it
    @new_interface_data = &parse_interface_ccl($thorn, @indata);
    
    #       Add the interface to the master interface database
    push (@interface_data, @new_interface_data);
    
  }
  
  return @interface_data;
}


#/*@@
#  @routine    parse_interface_ccl
#  @date       Wed Sep 16 15:07:11 1998
#  @author     Tom Goodale
#  @desc 
#  Parses an interface.ccl file and generates a database of the values.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub parse_interface_ccl
{
  local($thorn, @data) = @_;
  local($linenum, $line, $block, $type, $variable, $description, $nerrors);
  local($data, %interface_db);
  local($implementation);
  local($option,%options);
  local(%known_groups);
  local(%known_variables);
      
  #   The default block is private.
  $block = "PRIVATE";
  
  for($linenum = 0; $linenum < @data; $linenum++)
  {
    $line = $data[$linenum];
    
    #       Parse the line
    if($line =~ m/^\s*(PUBLIC|PROTECTED|PRIVATE)\s*$/i)
    {
      #           It's a new block.
      $block = "\U$1\E";
    } 
    elsif ($line =~ m/^\s*IMPLEMENTS:\s*([a-z]+[a-z_0-9]*)\s*$/i)
    {
      if($implementation == 0)
      {
	$implementation = $1;
	$interface_db{"\U$thorn\E IMPLEMENTS"} = $implementation;
      }
      else
      {
	print STDERR "Error:  Only one implements line allowed.\n";
      }
    }
    elsif ($line =~ m/^\s*(INHERITS|FRIEND)\s*:((\s*[a-zA-Z]+[a-zA-Z_0-9]*)*\s*)$/i)
    {
      $interface_db{"\U$thorn $1\E"} .= $2;
    }
    elsif ($line =~ m/^\s*(PUBLIC|PROTECTED|PRIVATE)\s*:\s*$/i)
    {
      $block = "\U$1\E";
    }
    elsif ($line =~ m/^\s*(INTEGER|REAL|CHAR)\s*([a-zA-Z]+[a-zA-Z_0-9]*)\s*(.*)\s*$/i)
    {
      $current_group = "$2";
      
      if($know_groups{"\U$current_group\E"})
      {
	print STDERR "Duplicate group $2 in thorn $thorn.\n";
	if($data[linenum+1] =~ m:\}:)
	{
	  print STDERR "...Skipping block ....\n";
	  $linenum++ until ($data[$linenum] =~ m:\}:);
	}
	next;
      }
      else
      {
	$know_groups{"\U$current_group\E"} = 1;
      }
      
      $interface_db{"\U$thorn $block GROUPS \E"} .= " $2";
      $interface_db{"\U$thorn GROUP $current_group\E VTYPE"} = "\U$1\E";
      %options = split(/\s+|\s*=\s*/, $3);
      
      # Parse the options
      foreach $option (keys %options)
      {
	if($option =~ m:DIM|DIMENSION:i)
	{
	  $interface_db{"\U$thorn GROUP $current_group\E DIM"} = $options{$option};
	}
	elsif($option =~ m:TYPE:i)
	{
	  $interface_db{"\U$thorn GROUP $current_group\E GTYPE"} = "\U$options{$option}\E";
	}
	else
	{
	  print STDERR "Unknown option $option in group $current_group of thorn $thorn.\n";
	}
      }

      # Put in defaults      
      if(! $interface_db{"\U$thorn GROUP $current_group\E GTYPE"})
      {
	$interface_db{"\U$thorn GROUP $current_group\E GTYPE"} = "SCALAR";
      }
      
      if(! $interface_db{"\U$thorn GROUP $current_group\E DIM"})
      {
	$interface_db{"\U$thorn GROUP $current_group\E DIM"} = 3;
      }
      
      # Check that it is a know group type
      if($interface_db{"\U$thorn GROUP $current_group\E GTYPE"} !~ m:SCALAR|GF|ARRAY:g)
      {
	print STDERR "Unknown GROUP TYPE " .
	  $interface_db{"\U$thorn GROUP $current_group\E GTYPE"} .
	    " for group $current_group of thorn $thorn.\n";
	if($data[linenum+1] =~ m:\}:)
	{
	  print STDERR "...Skipping block ....\n";
	  $linenum++ until ($data[$linenum] =~ m:\}:);
	}
	next;
      }	      
      
      # Fill in data for the scalars/arrays/functions
      $linenum++;
      if($data[$linenum] =~ m/^\s*\{\s*$/)
      {
	$linenum++;
	while($data[$linenum] !~ m:\}:i)
	{
	  @functions = split(/[^a-zA-Z_0-9]+/, $data[$linenum]);
	  foreach $function (@functions)
	  {
	    $function =~ s:\s*::g;
	    
	    if($function =~ m:[^\s]+:)
	    {
	      if(! $known_functions{"\U$function\E"})
	      {
		$known_functions{"\U$function\E"} = 1;
		
		$interface_db{"\U$thorn GROUP $current_group\E"} .= " $function";
	      }	    
	      else
	      {
		print STDERR "Duplicate variable $function in thorn $thorn\n";
	      }
	    }
	  }
	  $linenum++;
	}
      }
      else
      {
	# If no block, create a variable with the same name as group.
	$function = $current_group;
	if(! $known_functions{"\U$function\E"})
	{
	  $known_functions{"\U$function\E"} = 1;
	  
	  $interface_db{"\U$thorn GROUP $current_group\E"} .= " $function";
	}
	else
	{
	  print STDERR "Duplicate variable $function in thorn $thorn\n";
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
  
  return %interface_db;
}


sub print_interface_database
{
  local(%database) = @_;
  local($field);
  
  foreach $field ( sort keys %database ){
    print "$field has value $database{$field}\n";
  }
}

1;
