#/*@@
#  @file      ImpParamConsistency.pl
#  @date      Tue Mar  9 12:34:54 1999
#  @author    Tom Goodale
#  @desc 
#  Consistency checking for interface and parameter databases.
#  @enddesc 
#@@*/

sub CheckImpParamConsistency
{
  local($n_interface_data, @indata) = @_;
  local(%interface_database);
  local(%parameter_database);
  local(@thorns);
  local($thorn, $friend, $implementation, $other_thorn);
  local($range);

  # Extract the arguments
  %interface_database = @indata[0..2*$n_interface_data-1];
  %parameter_database = @indata[2*$n_interface_data..$#indata];
  
  @thorns = split(" ", $interface_database{"THORNS"});

  foreach $thorn (@thorns)
  {
#    print "Processing thorn $thorn\n";

    foreach $friend (split(" ", $parameter_database{"\U$thorn\E FRIEND implementations"}))
    {
#      print "Friend is $friend\n";
      # Find a thorn providing this implementation
      ($other_thorn) = split(" ", $interface_database{"IMPLEMENTATION \U$friend\E THORNS"});

#      print "Other thorn is $other_thorn\n";

      foreach $parameter (split(" ", $parameter_database{"\U$thorn FRIEND $friend\E variables"}))
      {
#	print "Parameter is $parameter\n";

	# Check if the parameter exists in the other thorn
	if($parameter_database{"\U$other_thorn $parameter\E type"})
	{
	  # Check that the parameter is in the protected block.
	  if($parameter_database{"\U$other_thorn PROTECTED\E variables"} =~ m:\b$parameter\b:i)
	  {
	    # Loop through all the added ranges.
	    for($range=1; 
		$range <= $parameter_database{"\U$thorn $parameter\E ranges"}; 
		$range++)
	    {
	      # Increment the number of ranges for the extended parameter
	      $parameter_database{"\U$other_thorn $parameter\E ranges"}++;

	      # Add in the range
	      $parameter_database{"\U$other_thorn $parameter\E range $parameter_database{\"\U$other_thorn $parameter\E ranges\"} range"} = $parameter_database{"\U$thorn $parameter\E range $range range"};

	      # Add in the range description
	      $parameter_database{"\U$other_thorn $parameter\E range $parameter_database{\"\U$other_thorn $parameter\E ranges\"} description"} = $parameter_database{"\U$thorn $parameter\E range $range description"};
	    }
	  }
	  else
	  {
	    print STDERR "Thorn $thorn attempted to EXTEND non-protected parameter $parameter from $friend\n";
	  }
	}
	else
	{
	  print STDERR "Thorn $thorn attempted to EXTEND non-existent parameter $parameter from $friend\n";
	}
      }
    }
  }

  return %parameter_database;
}

1;

