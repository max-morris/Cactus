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
    @indata = &read_file("$thorns{$thorn}/interface.ccl");
    
    #       Get the interface data from it
    @new_interface_data = &parse_interface_ccl($thorn, @indata);
    
    #       Add the interface to the master interface database
    push (@interface_data, @new_interface_data);
    
  }

  @interface_data = &cross_index_interface_data(scalar(keys %thorns), (keys %thorns), @interface_data);

  
  return @interface_data;
}



sub cross_index_interface_data
{
  local($n_thorns, @indata) = @_;
  local(@thorns);
  local(%interface_data);
  local(%implementations);
  local($implementation);

  @thorns = @indata[0..$n_thorns-1];
  %interface_data = @indata[$n_thorns..$#indata];

  foreach $thorn (@thorns)
  {
    $implementation = $interface_data{"\U$thorn\E IMPLEMENTS"};
    if($implementation =~ m:^\s*$:)
    {
      die "Thorn $thorn doesn't specify an implementation.\n";
    }

    $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"} .= " $thorn";
  
    $implementations{"\U$implementation\E"} = "$implementation";
  }

  $interface_data{"THORNS"} = join(" ", @thorns);

  foreach $implementation (keys %implementations)
  {
    $interface_data{"IMPLEMENTATIONS"} .= $implementations{"\U$implementation\E"} . " ";

    &check_interface_consistency($implementation, %interface_data);

    $interface_data{"IMPLEMENTATION \U$implementation\E ANCESTORS"} = &get_implementation_ancestors($implementation, %interface_data);

    $interface_data{"IMPLEMENTATION \U$implementation\E FRIENDS"} = &get_implementation_friends($implementation, %interface_data);
    
  }

  return %interface_data;
}

sub get_implementation_friends
{
  local($implementation, %interface_data);
  local($thorn);
  local($friend, $friends);
  local(%ancestors);

  $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"} =~ m:(\w+):;

  $thorn = $1;

  # Recurse.  This needs to be made robust against loops.
  foreach $ancestor (split(" ", $interface_data{"\U$thorn\E INHERITS"}))
  {
    $ancestors .= &get_implementation_ancestors($ancestor, %interface_data);
    $ancestors .= "$ancestor ";
  }
  
  # Uniquify the list.
  foreach $ancestor (split(" ", $ancestors))
  {
    $ancestors{"\U$ancestor\E"} = 1;
  }

  $ancestors = join(" ", %ancestors);

  return $ancestors;
}

sub get_implementation_ancestors
{
  local($implementation, %interface_data);
  local($thorn);
  local($ancestor, $ancestors);
  local(%ancestors);

  $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"} =~ m:(\w+):;

  $thorn = $1;

  # Recurse.  This needs to be made robust against loops.
  foreach $ancestor (split(" ", $interface_data{"\U$thorn\E INHERITS"}))
  {
    $ancestors .= &get_implementation_ancestors($ancestor, %interface_data);
    $ancestors .= "$ancestor ";
  }
  
  # Uniquify the list.
  foreach $ancestor (split(" ", $ancestors))
  {
    $ancestors{"\U$ancestor\E"} = 1;
  }

  $ancestors = join(" ", %ancestors);

  return $ancestors;
}

sub check_interface_consistency
{
  local($implementation, %interface_data) = @_;
  local(@thorns);
  local($thorn);
  local($thing);
  local(%inherits);
  local(%friend);
  local(%public_groups);
  local(%private_groups);
  local(%variables);
  local($n_errors);
  local($group);
  local(%attributes);
    
  # Find out which thorns provide this implementation.
  @thorns = split(" ", $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"});

  if(scalar(@thorns) > 1)
  {
    foreach $thorn (@thorns)
    {
      # Record the inheritance
      foreach $thing (split(" ", $interface_data{"\U$thorn\E INHERITS"}))
      {
	if($thing =~ m:\w:)
	{
	  $inherits{"\U$thing\E"} .= "$thorn ";
	}
      }

      # Record the friends
      foreach $thing (split(" ", $interface_data{"\U$thorn\E FRIEND"}))
      {
	if($thing =~ m:\w:)
	{
	  $friend{"\U$thing\E"} .= "$thorn ";
	}
      }
  
      # Record the piblic groups
      foreach $thing (split(" ", $interface_data{"\U$thorn\E PUBLIC GROUPS"}))
      {
	if($thing =~ m:\w:)
	{
	  $public_groups{"\U$thing\E"} .= "$thorn ";
	}
      }

      # Record the protected groups
      foreach $thing (split(" ", $interface_data{"\U$thorn\E PROTECTED GROUPS"}))
      {
	if($thing =~ m:\w:)
	{
	  $protected_groups{"\U$thing\E"} .= "$thorn ";
	}
      }
    }

    $n_thorns = @thorns;

    # Check the consistency of the inheritance
    foreach $thing (keys %inherits)
    {
      if(split(" ", $inherits{$thing}) != $n_thorns)
      {
	if(!$n_errors)
	{
	  print STDERR "Inconsistent implementations of $implementation\n";
	  print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	}
	print STDERR "    Not all inherit:         $thing\n";
	$n_errors++;
      }
    }

    # Check the consistency of the friendships
    foreach $thing (keys %friend)
    {
      if(split(" ", $friend{$thing}) != $n_thorns)
      {
	if(!$n_errors)
	{
	  print STDERR "Inconsistent implementations of $implementation\n";
	  print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	}
	print STDERR "    Not all are friends of : $thing\n";
	$n_errors++;
      }
    }

    # Check the consistency of the public groups
    foreach $thing (keys %public_groups)
    {
      if(split(" ", $public_groups{$thing}) != $n_thorns)
      {
	if(!$n_errors)
	{
	  print STDERR "Inconsistent implementations of $implementation\n";
	  print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	}
	print STDERR "    Not all declare public group: $thing\n";
	$n_errors++;
      }
    }

    # Check the consistency of the protected groups
    foreach $thing (keys %protected_groups)
    {
      if(split(" ", $protected_groups{$thing}) != $n_thorns)
      {
	if(!$n_errors)
	{
	  print STDERR "Inconsistent implementations of $implementation\n";
	  print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	}
	print STDERR "    Not all declare potected group: $thing\n";
	$n_errors++;
      }
    }

    # Stop if any errors discovered so far.
    if($n_errors)
    {
      print STDERR "$n_errors Errors found, please fix before continuing.\n";
      exit;
    }

    # Check consistancy of group definitions
    foreach $group ((keys %public_groups), (keys %protected_groups))
    {
      %variables = ();
      %attributes = ();

      foreach $thorn (@thorns)
      {
	# Remember which variables are defined in this group.
	foreach $thing (split(" ",$interface_data{"\U$thorn GROUP $group\E"}))
	{
	  $variables{"\U$thing\E"} .= "$thorn ";
	}

	# Check variable type definition.
	if($attributes{"VTYPE"})
	{
	  if($attributes{"VTYPE"} ne $interface_data{"\U$thorn GROUP $group\E VTYPE"})
	  {
	    if(!$n_errors)
	    {
	      print STDERR "Inconsistent implementations of $implementation\n";
	      print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	    }
	    print STDERR "      Group $group has inconsistent variable type.\n";
	    $n_errors++;
	  }
	}
	else
	{
	  $attributes{"VTYPE"} = $interface_data{"\U$thorn GROUP $group\E VTYPE"};
	}

	# Check group type definition.
	if($attributes{"GTYPE"})
	{
	  if($attributes{"GTYPE"} ne $interface_data{"\U$thorn GROUP $group\E GTYPE"})
	  {
	    if(!$n_errors)
	    {
	      print STDERR "Inconsistent implementations of $implementation\n";
	      print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	    }
	    print STDERR "      Group $group has inconsistent group type.\n";
	    $n_errors++;
	  }
	}
	else
	{
	  $attributes{"GTYPE"} = $interface_data{"\U$thorn GROUP $group\E GTYPE"};
	}

	# Check the dimensions are consistant
	if($attributes{"DIM"} && $attributes{"GTYPE"} ne "SCALAR")
	{
	  if($attributes{"DIM"} ne $interface_data{"\U$thorn GROUP $group\E DIM"})
	  {
	    if(!$n_errors)
	    {
	      print STDERR "Inconsistent implementations of $implementation\n";
	      print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	    }
	    print STDERR "      Group $group has inconsistent dimension.\n";
	    $n_errors++;
	  }
	}	  
	else
	{
	  $attributes{"DIM"} = $interface_data{"\U$thorn GROUP $group\E DIM"};
	}
      }
    }
  }
  else
  {
    # No need to do a consistency check if only one thorn 
    # provides this implementation.

  }

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
      
      if($known_groups{"\U$current_group\E"})
      {
	print STDERR "Duplicate group $2 in thorn $thorn.\n";
	if($data[linenum+1] =~ m:\{:)
	{
	  print STDERR "...Skipping block ....\n";
	  $linenum++ until ($data[$linenum] =~ m:\}:);
	}
	next;
      }
      else
      {
	$known_groups{"\U$current_group\E"} = 1;
      }
      
      $interface_db{"\U$thorn $block GROUPS\E"} .= " $2";
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
      if($interface_db{"\U$thorn GROUP $current_group\E GTYPE"} !~ m:SCALAR|GF|ARRAY:)
      {
	print STDERR "Unknown GROUP TYPE " .
	  $interface_db{"\U$thorn GROUP $current_group\E GTYPE"} .
	    " for group $current_group of thorn $thorn.\n";
	if($data[linenum+1] =~ m:\{:)
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
	      if(! $known_variables{"\U$function\E"})
	      {
		$known_variables{"\U$function\E"} = 1;
		
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
	if(! $known_variables{"\U$function\E"})
	{
	  $known_variables{"\U$function\E"} = 1;
	  
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
