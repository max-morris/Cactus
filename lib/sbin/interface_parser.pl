#! /usr/bin/perl -w

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
    print "   $thorn\n";
    #       Get the arrangement name for the thorn
    $thorns{$thorn} =~ m:.*/arrangements/([^/]*)/[^/]*:;
    $arrangement = $1;

    #       Read the data
    @indata = &read_file("$thorns{$thorn}/interface.ccl");
    
    #       Get the interface data from it
    @new_interface_data = &parse_interface_ccl($arrangement,$thorn, @indata);

    &PrintInterfaceStatistics($thorn, @new_interface_data);
    
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
  local(%ancestors);
  local(%friends);

  @thorns = @indata[0..$n_thorns-1];
  %interface_data = @indata[$n_thorns..$#indata];

  foreach $thorn (@thorns)
  {
    $implementation = $interface_data{"\U$thorn\E IMPLEMENTS"};
    if($implementation =~ m:^\s*$:)
    {
      $message = "Thorn $thorn doesn't specify an implementation";
      &CST_error(0,$message,__LINE__,__FILE__);
      next;
    }

    # Put if statement around this to prevent perl -w from complaining.
    if($interface_data{"IMPLEMENTATION \U$implementation\E THORNS"})
    {
      $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"} .= "$thorn ";
    }
    else
    {
      $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"} = "$thorn ";
    }

    $implementations{"\U$implementation\E"} = "$implementation";
  }

  $interface_data{"THORNS"} = join(" ", @thorns);

  foreach $implementation (keys %implementations)
  {
    # Put if statement around this to prevent perl -w from complaining.
    if($interface_data{"IMPLEMENTATIONS"})
    {
      $interface_data{"IMPLEMENTATIONS"} .= $implementations{"\U$implementation\E"} . " ";
    }
    else
    {
      $interface_data{"IMPLEMENTATIONS"} = $implementations{"\U$implementation\E"} . " ";
    }

    &check_interface_consistency($implementation, %interface_data);

    %ancestors = &get_implementation_ancestors($implementation, 0, %interface_data);

    $interface_data{"IMPLEMENTATION \U$implementation\E ANCESTORS"} = join(" ",( keys %ancestors));


    $interface_data{"IMPLEMENTATION \U$implementation\E FRIENDS"} = &get_friends_of_me($implementation, scalar(keys %implementations), (keys %implementations),%interface_data);

  }

  foreach $implementation (keys %implementations)
  {
    %friends = &get_implementation_friends($implementation, 0, %interface_data);
    $interface_data{"IMPLEMENTATION \U$implementation\E FRIENDS"} = join(" ",( keys %friends));
  }

  return %interface_data;
}


sub get_friends_of_me
{
  local($implementation, $n_implementations,@indata) = @_;
  local(@implementations);
  local(%interface_data);
  local($other_implementation);
  local($thorn);
  local($friend,$friends);

  @implementations = @indata[0..$n_implementations-1];
  %interface_data = @indata[$n_implementations..$#indata];

  # Initialise to stop perl -w from complaining.
  $friends = "";

  foreach $other_implementation (@implementations)
  {

    $interface_data{"IMPLEMENTATION \U$other_implementation\E THORNS"} =~ m:(\w+):;

    $thorn = $1;

    foreach $friend (split(" ", $interface_data{"\U$thorn\E FRIEND"}))
    {
      if($friend =~ m:$implementation:i)
      {
	  $friends .= "$other_implementation ";	
      }
    }
  }

  return $friends;
}
  

sub get_implementation_friends
{
  local($implementation, $n_friends, @indata) = @_;
  local(%friends);
  local(%interface_data);
  local($thorn);
  local($friend, $friends);
  local($friends_of_me);
  local($other_implementation);

  if($n_friends > 0)
  {
    %friends = @indata[0..2*$n_friends-1];
    %interface_data = @indata[2*$n_friends..$#indata];
  }
  else
  {
    %friends = ();
    %interface_data = @indata;
  }

  $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"} =~ m:(\w+):;

  $thorn = $1;

  # Recurse
  foreach $friend (split(" ", $interface_data{"\U$thorn\E FRIEND"}), 
		   split(" ", $interface_data{"IMPLEMENTATION \U$implementation\E FRIENDS"}))
  {
    if(! $friends{"\U$friend\E"})
    {
      $friends{"\U$friend\E"} = 1;
      if(! $interface_data{"IMPLEMENTATION \U$friend\E THORNS"})
      {
	print "$implementation is friends with $friend - non-existent implementation\n";
	$CST_errors++;
	next;
      }
      %friends = &get_implementation_friends($friend, scalar(keys %friends), %friends,%interface_data);
    }
  }
  
  return %friends;

}

sub get_implementation_ancestors
{
  local($implementation, $n_ancestors, @indata) = @_;
  local(%ancestors);
  local(%interface_data);
  local($thorn);
  local($ancestor, $ancestors);

  if($n_ancestors > 0)
  {
    %ancestors = @indata[0..2*$n_ancestors-1];
    %interface_data = @indata[2*$n_ancestors..$#indata];
  }
  else
  {
    %ancestors = ();
    %interface_data = @indata;
  }

  $interface_data{"IMPLEMENTATION \U$implementation\E THORNS"} =~ m:(\w+):;

  $thorn = $1;

  # Recurse.
  foreach $ancestor (split(" ", $interface_data{"\U$thorn\E INHERITS"}))
  {
    if(! $ancestors{"\U$ancestor\E"})
    {
      $ancestors{"\U$ancestor\E"} = 1;
      if(! $interface_data{"IMPLEMENTATION \U$ancestor\E THORNS"})
      {
	print "$implementation inherits from $ancestor - non-existent implementation\n";
	$CST_errors++;
	next;
      }
      %ancestors = &get_implementation_ancestors($ancestor, scalar(keys %ancestors), %ancestors,%interface_data);
    }
  }
  
  return %ancestors;
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
	  # Put if statement around this to prevent perl -w from complaining.
	  if($inherits{"\U$thing\E"})
	  {
	    $inherits{"\U$thing\E"} .= "$thorn ";
	  }
	  else
	  {
	    $inherits{"\U$thing\E"} = "$thorn ";
	  }
	}
      }

      # Record the friends
      foreach $thing (split(" ", $interface_data{"\U$thorn\E FRIEND"}))
      {
	if($thing =~ m:\w:)
	{
	  # Put if statement around this to prevent perl -w from complaining.
	  if($friend{"\U$thing\E"})
	  {
	    $friend{"\U$thing\E"} .= "$thorn ";
	  }
	  else
	  {
	    $friend{"\U$thing\E"} = "$thorn ";
	  }	    
	}
      }
  
      # Record the public groups
      foreach $thing (split(" ", $interface_data{"\U$thorn\E PUBLIC GROUPS"}))
      {
	if($thing =~ m:\w:)
	{
	  # Put if statement around this to prevent perl -w from complaining.
	  if($public_groups{"\U$thing\E"})
	  {
	    $public_groups{"\U$thing\E"} .= "$thorn ";
	  }
	  else
	  {
	    $public_groups{"\U$thing\E"} = "$thorn ";
	  }
	}
      }

      # Record the protected groups
      foreach $thing (split(" ", $interface_data{"\U$thorn\E PROTECTED GROUPS"}))
      {
	if($thing =~ m:\w:)
	{
	  # Put if statement around this to prevent perl -w from complaining.
	  if($protected_groups{"\U$thing\E"})
	  {
	    $protected_groups{"\U$thing\E"} .= "$thorn ";
	  }
	  else
	  {
	    $protected_groups{"\U$thing\E"} = "$thorn ";
	  }
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
	    $message = "Inconsistent implementations of $implementation. Implemented by thorns " . join(" ", @thorns);
	    &CST_error(0,$message,__LINE,__FILE__);
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
	  $message = "Inconsistent implementations of $implementation\n    Implemented by thorns " . join(" ", @thorns) . "\n";
	  &CST_error(0,$message,__LINE,__FILE__);
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
	&CST_error(0,"    Not all declare public group: $thing",__LINE__,__FILE__);
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
	  print STDERR "Inconsistent implementations of $implementation\n       Implemented by thorns " . join(" ", @thorns) . "\n";
	}
	&CST_error(0,"    Not all declare protected group: $thing\n",__LINE__,__FILE__);
	$n_errors++;
      }
    }

    # Stop if any errors discovered so far.
#    if($n_errors)
#    {
#      print STDERR "$n_errors Errors found, please fix before continuing.\n";
#      exit;
#    }

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
	  # Put if statement around this to prevent perl -w from complaining.
	  if($variables{"\U$thing\E"})
	  {
	    $variables{"\U$thing\E"} .= "$thorn ";
	  }
	  else
	  {
	    $variables{"\U$thing\E"} = "$thorn ";
	  }
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

	# Check the number of time levels is consistent.
	if($attributes{"TIMELEVELS"})
	{
	  if($attributes{"TIMELEVELS"} ne $interface_data{"\U$thorn GROUP $group\E TIMELEVELS"})
	  {
	    if(!$n_errors)
	    {
	      print STDERR "Inconsistent implementations of $implementation\n";
	      print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	    }
	    print STDERR "      Group $group has inconsistent time levels.\n";
	    $n_errors++;
	  }
	}
	else
	{
	  $attributes{"TIMELEVELS"} = $interface_data{"\U$thorn GROUP $group\E TIMELEVELS"};
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
	# Check the staggering are consistant
	if($attributes{"STYPE"})
	{
	  if($attributes{"STYPE"} ne $interface_data{"\U$thorn GROUP $group\E STYPE"})
	  {
	    if(!$n_errors)
	    {
	      print STDERR "Inconsistent implementations of $implementation\n";
	      print STDERR "    Implemented by thorns " . join(" ", @thorns) . "\n";
	    }
	    print STDERR "      Group $group has inconsistent staggering type.\n";
	    $n_errors++;
	  }
	}	  
	else
	{
	  $attributes{"STYPE"} = $interface_data{"\U$thorn GROUP $group\E STYPE"};
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
  local($arrangement, $thorn, @data) = @_;
  local($line_number, $line, $block, $type, $variable, $description);
  local($data, %interface_db);
  local($implementation);
  local($option,%options);
  local(%known_groups);
  local(%known_variables);
      

  # Initialise some stuff to prevent perl -w from complaining.

  $interface_db{"\U$thorn INHERITS\E"} = "";
  $interface_db{"\U$thorn FRIEND\E"} = "";
  $interface_db{"\U$thorn PUBLIC GROUPS\E"} = "";
  $interface_db{"\U$thorn PROTECTED GROUPS\E"} = "";
  $interface_db{"\U$thorn PRIVATE GROUPS\E"} = "";
  $interface_db{"\U$thorn USES HEADER\E"} = "";
  $interface_db{"\U$thorn ARRANGEMENT\E"} = "$arrangement";
  
  #   The default block is private.
  $block = "PRIVATE";
  
  for($line_number = 0; $line_number < @data; $line_number++)
  {
    $line = $data[$line_number];
    
    #       Parse the line
    if($line =~ m/^\s*(PUBLIC|PROTECTED|PRIVATE)\s*$/i)
    {
      #           It's a new block.
      $block = "\U$1\E";
    } 
    elsif ($line =~ m/^\s*IMPLEMENTS:\s*([a-z]+[a-z_0-9]*)\s*$/i)
    {
      if(!$implementation)
      {
	$implementation = $1;
	$interface_db{"\U$thorn\E IMPLEMENTS"} = $implementation;
      }
      else
      {
	  $message = "Only one implements line allowed in $thorn";
	  &CST_error(0,$message,__LINE__,__FILE__);
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
    elsif ($line =~ m/^\s*(CCTK_)?(INT|INT2|INT4|INT8|REAL|REAL4|REAL8|REAL16|CHAR|COMPLEX)\s*([a-zA-Z]+[a-zA-Z_0-9]*)\s*(.*)\s*$/i)
    {
      $current_group = "$3";
      
      if($known_groups{"\U$current_group\E"})
      {
	$message = "Duplicate group $3 in thorn $thorn";
	&CST_error(0,$message,__LINE__,__FILE__);
	if($data[line_number+1] =~ m:\{:)
	{
	    $message = "Skipping interface block";
	    &CST_error(1,$message,__LINE__,__FILE__);
	    $line_number++ until ($data[$line_number] =~ m:\}:);
	}
	next;
      }
      else
      {
	$known_groups{"\U$current_group\E"} = 1;

	# Initialise some stuff to prevent perl -w from complaining.
	$interface_db{"\U$thorn GROUP $current_group\E"} = "";
      }
      
      $interface_db{"\U$thorn $block GROUPS\E"} .= " $3";
      $interface_db{"\U$thorn GROUP $current_group\E VTYPE"} = "\U$2\E";
      %options = split(/\s*=\s*|\s+/, $4);
      
      # Parse the options
      foreach $option (keys %options)
      {
	if($option =~ m:DIM|DIMENSION:i)
	{
	  $interface_db{"\U$thorn GROUP $current_group\E DIM"} = $options{$option};
	}
	elsif($option =~ m:STAGGER:i)
	{
	  $interface_db{"\U$thorn GROUP $current_group\E STYPE"} = "\U$options{$option}\E";
	}
	elsif($option =~ m:TYPE:i)
	{
	  $interface_db{"\U$thorn GROUP $current_group\E GTYPE"} = "\U$options{$option}\E";
	}
	elsif($option =~ m:TIMELEVELS:i)
	{
	  $interface_db{"\U$thorn GROUP $current_group\E TIMELEVELS"} = "\U$options{$option}\E";
	}
	else
	{
	  $message = "Unknown option $option in group $current_group of thorn $thorn";
	  &CST_error(0,$message,__LINE__,__FILE__);
	}
      }

      # Put in defaults      
      if(! $interface_db{"\U$thorn GROUP $current_group\E GTYPE"})
      {
	$interface_db{"\U$thorn GROUP $current_group\E GTYPE"} = "SCALAR";
      }

      if($interface_db{"\U$thorn GROUP $current_group\E GTYPE"} eq "SCALAR")
      {
	$interface_db{"\U$thorn GROUP $current_group\E DIM"} = 1;
      }

      if(! $interface_db{"\U$thorn GROUP $current_group\E DIM"})
      {
	$interface_db{"\U$thorn GROUP $current_group\E DIM"} = 3;
      }

      if(! $interface_db{"\U$thorn GROUP $current_group\E TIMELEVELS"})
      {
	$interface_db{"\U$thorn GROUP $current_group\E TIMELEVELS"} = 1;
      }

      if(! $interface_db{"\U$thorn GROUP $current_group\E STYPE"})
      {
	$interface_db{"\U$thorn GROUP $current_group\E STYPE"} = "NONE";
      }
      
      # Check that it is a known group type
      if($interface_db{"\U$thorn GROUP $current_group\E GTYPE"} !~ m:SCALAR|GF|ARRAY:)
      {
	  $message =  "Unknown GROUP TYPE " .
	  $interface_db{"\U$thorn GROUP $current_group\E GTYPE"} .
	    " for group $current_group of thorn $thorn";
	  &CST_error(0,$message,__LINE,__FILE__);
	  if($data[line_number+1] =~ m:\{:)
	  {
	      $message = "Skipping interface block in $thorn";
	      &CST_error(1,$message,__LINE__,__FILE__);
	      $line_number++ until ($data[$line_number] =~ m:\}:);
	  }
	next;
      }	      
      
      # Fill in data for the scalars/arrays/functions
      $line_number++;
      if($data[$line_number] =~ m/^\s*\{\s*$/)
      {
	$line_number++;
	while($data[$line_number] !~ m:\}:i)
	{
	  @functions = split(/[^a-zA-Z_0-9]+/, $data[$line_number]);
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
		$message = "Duplicate variable $function in thorn $thorn";
		&CST_error(0,$message,__LINE__,__FILE__);
	      }
	    }
	  }
	  $line_number++;
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
	  $message = "Duplicate variable $function in thorn $thorn";
	  &CST_error(0,$message,__LINE__,__FILE__);
	}
	
	# Decrement the line number, since the line is the first line of the next CCL statement.
	$line_number--;
	
      }
    }
    elsif ($line =~ m/^\s*(USES\s*INCLUDE)S?\s*:\s*(.*)\s*$/)
    {
      $interface_db{"\U$thorn USES HEADER\E"} .= " $2";      
    }
    elsif ($line =~ m/^\s*(INCLUDES)\s*:\s*(.*)\s+in\s+(.*)\s*$/)
    {
      $header = $2;
      $header =~ s/ //g;
      $interface_db{"\U$thorn ADD HEADER\E"} .= " $header";      
#      print "Adding $header to $3\n";
      $interface_db{"\U$thorn ADD HEADER $header TO\E"} = $3;      
    }
    else
    {
      if($line =~ m:\{:)
      {
	$message = "...Skipping interface block with missing keyword....";
	&CST_error(0,$message,__LINE__,__FILE__);

	$line_number++ until ($data[$line_number] =~ m:\}:);
      }
      else
      {
	$message = "Unknown line $line";
	&CST_error(0,$message,__LINE__,__FILE__);
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

#/*@@
#  @routine    PrintInterfaceStatistics
#  @date       Sun Sep 19 13:03:23 1999
#  @author     Tom Goodale
#  @desc 
#  Prints out some statistics about a thorn's interface.ccl  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub PrintInterfaceStatistics
{
  local($thorn, %interface_database) = @_;
  local($block);
  local($sep);

  print "           Implements: " . $interface_database{"\U$thorn IMPLEMENTS"} . "\n";

  if($interface_database{"\U$thorn INHERITS"} ne "")
  {
    print "           Inherits:  " . $interface_database{"\U$thorn INHERITS"} . "";
  }

  if($interface_database{"\U$thorn FRIEND"} ne "")
  {
    print "           Friend of: " . $interface_database{"\U$thorn FRIEND"} . "";
  }

  $sep = "           ";
  foreach $block ("Public", "Protected", "Private")
  {
    print $sep . scalar(split(" ", $interface_database{"\U$thorn $block\E GROUPS"})) . " $block";
    $sep = ", ";
  }

  print " variable groups\n";

  return;
}

1;
