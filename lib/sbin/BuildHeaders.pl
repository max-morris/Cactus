#! /usr/bin/perl -s

#/*@@
#  @routine BuildHeaders
#  @date Sun 13 Sep 1999
#  @author Gabrielle Allen
#  @desc
#  Creates the dynamic header files requested in interface.ccl files
#  and writes them into the Bindings include directory
#  @enddesc
#  @calls
#  @calledby
#  @history
#  @endhistory
#@@*/


sub BuildHeaders
{
  local($cctk_home,$bindings_dir,%database) = @_;
  local($start_dir,$thorn,$inc_file,$inc_file1,$inc_file2);

  $start_dir = `pwd`;
  chdir $bindings_dir;
  chdir include;

# First delete all global include files since we will be appending
  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    foreach $inc_file (split(" ",$interface_database{"\U$thorn USES HEADER"}))
    {
      if (-e $inc_file)
      {
	system("rm $inc_file");
      }
    }
  }

# Create all the global include files used by thorns
  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    foreach $inc_file (split(" ",$interface_database{"\U$thorn USES HEADER"}))
    {
      if (!-e $inc_file)
      {
	open(OUT,">$inc_file") || die "Cannot open $inc_dir";
	print OUT "/* Include file $inc_file used by $thorn */\n";
	close OUT;
      }
      else
      {
	open(OUT,">>$inc_file") || die "Cannot open $inc_dir";
	print OUT "/* Include file $inc_file used by $thorn */\n";
	close OUT;
      }
    }
  }

# Add the local headers from thorns
  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {

    $arrangement = $interface_database{"\U$thorn ARRANGEMENT"};

    foreach $inc_file1 (split(" ",$interface_database{"\U$thorn ADD HEADER"}))
    {
      if ($inc_file1 !~ /^\s*$/)
      {
	$inc_file1 =~ s/ //g;
	$inc_file2 = $interface_database{"\U$thorn ADD HEADER $inc_file1 TO"};

	# Check the global include file is already there
	if (! -e $inc_file2)
	{
	  $message = "No thorn requires $inc_file2 written to by $thorn\n";
	  &CST_error(2,$message,__LINE__,__FILE__);
	}
	else
	{
	  # Write information to the global include file
	  open(OUT,">>$inc_file2") || die "Cannot open $inc_dir2";
	  print OUT "/* Including file $inc_file1 from $thorn */\n";
	
	  # Now have to find the include file and copy it
	  if (-e "$cctk_home/arrangements/$arrangement/$thorn/src/$inc_file1")
	  {
	    open(HEADER,"<$cctk_home/arrangements/$arrangement/$thorn/src/$inc_file1");
	    while (<HEADER>)
	    {
	      print OUT;
	    }
	    print OUT "\n\n\n";
	    close HEADER;
	  }
	  elsif (-e "$cctk_home/arrangements/$arrangement/$thorn/src/include/$inc_file1")
	  {
	    open(HEADER,"<$cctk_home/arrangements/$arrangement/$thorn/src/$inc_file1");
	    while (<HEADER>)
	    {
	      print OUT;
	    }
	    print OUT "\n\n\n";
	    close HEADER;
	  }
	  else
	  {
	    $message = "Include file $inc_file1 not found in $arrangement/$thorn\n";
	    &CST_error(0,$message,__LINE__,__FILE__);
	  }
	  print OUT "/* End of include file $inc_file1 from $thorn */\n";
	  close OUT;
	}
      }
    }
  }

  chdir $start_dir;
  return;

}

1;

