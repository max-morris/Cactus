#! /usr/bin/perl
#/*@@
#  @file      GridFuncStuff.pl
#  @date      Tue Jan 12 11:07:45 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc 
#  @version $Id$
#@@*/

#/*@@
#  @routine    CreateGroups
#  @date       Tue Jan 12 11:08:19 1999
#  @author     Tom Goodale
#  @desc 
#  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub CreateGroups
{
  local(%interface_database) = @_;
  local(@interfaces);
  local(%thorns);
  local(@group_initialisers);
  local(@indata);
    

  @interfaces = split(" ", $interface_database{"IMPLEMENTATIONS"});

  foreach $interface (@interfaces)
  {
    @indata = &create_interface_group_initialisers($interface, %interface_database);

    push(@group_initialisers, @indata);

    foreach $thorn (split(" ",$interface_database{"IMPLEMENTATION \U$interface\E THORNS"}))
    {
      $thorns{"\U$thorn\E"} = 1;
    }
  }

  foreach $thorn (keys %thorns)
  {
    
    @indata = &create_thorn_group_initialisers($thorn, "PRIVATE", %interface_database);

    push(@group_initialisers, @indata);
    
  }

  @group_initialisers = &sort_groups(@group_initialisers);

  return @group_initialisers;
}

#/*@@
#  @routine    create_interface_group_initialisers
#  @date       Tue Jan 12 11:08:41 1999
#  @author     Tom Goodale
#  @desc 
#  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_interface_group_initialisers
{
  local($interface, %interface_database) = @_;
  local($thorn);
  local(@definitions);
  local(@indata);

  
  $interface_database{"IMPLEMENTATION \U$interface\E THORNS"} =~ m:([^ ]+):;

  $thorn = $1;

  @indata = &create_thorn_group_initialisers($thorn, "PUBLIC", %interface_database);

  push(@definitions, @indata);

  @indata = &create_thorn_group_initialisers($thorn, "PROTECTED", %interface_database);

  push(@definitions, @indata);

  return @definitions;
  
}
  
#/*@@
#  @routine    create_thorn_group_initialisers
#  @date       Tue Jan 12 11:09:08 1999
#  @author     Tom Goodale
#  @desc 
#  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/
sub create_thorn_group_initialisers
{
  local($thorn, $block, %interface_database) = @_;
  local(@definitions);
  local($base_name);
  local($group);
  local($variable,@variables);
  if($block eq "PRIVATE")
  {
    $base_name = "\$$thorn";
  }
  else
  {
    $base_name = $interface_database{"\U$thorn\E IMPLEMENTS"};
  }    
  
  foreach $group (split(" ", $interface_database{"\U$thorn $block GROUPS"}))
  {
    @variables = split(" ", $interface_database{"\U$thorn GROUP $group\E"});

    $line  = "  CCTK_CreateGroup(\"\U$base_name::$group\E\",\n" 
           . "                   \"" . $interface_database{"\U$thorn GROUP $group\E GTYPE"} . "\",\n"
	   . "                   \"" . $interface_database{"\U$thorn GROUP $group\E VTYPE"} . "\",\n"
           . "                   ". scalar(@variables);
    foreach $variable (@variables)
    {
      $line .= ",\n                   \"\U$variable\E\"";
    }

    $line  .= ");\n\n";

    push(@definitions, $line);
  }

  return @definitions;
    

}

#/*@@
#  @routine    sort_groups
#  @date       Tue Jan 12 11:09:26 1999
#  @author     Tom Goodale
#  @desc 
#  
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/
sub sort_groups
{
  local(@group_initialisers) = @_;

  return @group_initialisers;
}


sub GetThornArguments
{
  local($this_thorn, $block, %interface_database) = @_;
  local(%arguments);
  local(@other_imps);
  local($my_imp);
  local($imp);
  local($thorn, $group, $variable, $vtype, $gtype, $type);

  $my_imp = $interface_database{"\U$this_thorn IMPLEMENTS"};

  if($block eq "PUBLIC")
  {
    @other_imps = $interface_data{"IMPLEMENTATION \U$my_imp\E ANCESTORS"};
  }
  elsif($block eq "PROTECTED")
  {
    @other_imps = $interface_data{"IMPLEMENTATION \U$my_imp\E FRIENDS"};
  }
  elsif($block eq "PRIVATE")
  {
    @other_imps = ();
  }
  else
  {
    die "Unknown block type $block!!!\n";
  }

#  print "Thorn is $this_thorn, implementation $my_imp, block is $block\n";


  foreach $imp (@other_imps,$my_imp)
  {

    next if (! defined $imp);

    $interface_database{"IMPLEMENTATION \U$imp\E THORNS"} =~ m:([^ ]*):;

    $thorn = $1;

#    print "This thorn is $thorn, implementation $imp\n";

    foreach $group (split(" ",$interface_database{"\U$thorn $block GROUPS\E"}))
    {
      $vtype = $interface_database{"\U$thorn GROUP $group VTYPE\E"};
      $gtype = $interface_database{"\U$thorn GROUP $group GTYPE\E"};

      $type = "$vtype";

      if($gtype eq "GF" || $gtype eq "ARRAY")
      {
	$type .= " (";
	$sep = "";
	for($dim =0; $dim < $interface_database{"\U$thorn GROUP $group DIM\E"}; $dim++)
	{
	  $type .= "$sep$group$dim";
	  $sep = ",";
	  $arguments{"$group$dim"} = "STORAGESIZE($thorn:$group, $dim)";
	}
	$type .= ")";
      }

      $type .= "!$thorn:$group";

#      print "Group is $group, resulting type is $type\n";

      foreach $variable (split(" ", $interface_database{"\U$thorn GROUP $group\E"}))
      {
       $arguments{$variable} = $type;
      }
    }
  }

  return %arguments;
}


sub CreateFortranArgumentDeclarations
{
  local(%arguments) = @_;
  local($argument);
  local(@declarations) = ();

  # Put all storage arguments first.
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} =~ m:STORAGESIZE:)
    {
      push(@declarations, "INTEGER $argument");
    }
  }
  
  # Now deal with the rest of the arguments
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*):;

      if($1 eq "CHAR")
      {
	push(@declarations, "CHARACTER $argument$2");
      }
      elsif ($1 eq REAL)
      {
	push(@declarations, "REAL $argument$2");
      }
      elsif ($1 eq COMPLEX)
      {
	push(@declarations, "COMPLEX $argument$2");
      }
      elsif ($1 eq INTEGER)
      {
	push(@declarations, "INTEGER $argument$2");
      }
      else
      {
	print STDERR "Unknown argument type $1\n";
      }
    }
  }

  return @declarations;
    
}

sub CreateFortranArgumentList
{
  local(%arguments) = @_;
  local($argument);
  local($argumentlist) = "";
  local($sep);

  $sep = "";
  # Put all storage arguments first.
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} =~ m:STORAGESIZE:)
    {
      $argumentlist .= "$sep$argument";
      $sep = ",";
    }
  }

  # Now deal with the rest of the arguments
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $argumentlist .= "$sep$argument";
      $sep = ",";
    }
  }

  return $argumentlist;
}

sub CreateCArgumentStatics
{
  local(%arguments) = @_;
  local($argument);
  local(@declarations) = ();

  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      push(@declarations, "static int CCTKARGNUM_$argument = -1");
    }
  }

  return @declarations;
}  


sub CreateCArgumentInitialisers
{
  local(%arguments) = @_;
  local($argument);
  local(@initialisers) = ();

  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*):;

      push(@initialisers, "if(CCTKARGNUM_$argument == -1) CCTKARGNUM_$argument = CCTK_GetVarNum(\"$3\")");
    }
  }

  return @initialisers;
}

sub CreateCArgumentPrototype
{
  local(%arguments) = @_;
  local($argument);
  local($prototype) = "";
  local($sep);
  
  $sep = "";

  # Put all storage arguments first.
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} =~ m:STORAGESIZE:)
    {
      $prototype .= "$sep"."int *";
      $sep = ",";
    }
  }
  
  # Now deal with the rest of the arguments
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*):;

      if($1 eq "CHAR")
      {
	$prototype .="$sep". "char *";
	$sep = ",";
      }
      elsif ($1 eq REAL)
      {
	$prototype .="$sep". "Double *";
	$sep = ",";	
      }
      elsif ($1 eq COMPLEX)
      {
	$prototype .="$sep". "Complex *";
	$sep = ",";
      }
      elsif ($1 eq INTEGER)
      {
	$prototype .="$sep". "int *";
	$sep = ",";
      }
      else
      {
	print STDERR "Unknown argument type $1\n";
      }
    }
  }

  return $prototype;
}


sub CreateCArgumentList
{
  local(%arguments) = @_;
  local($argument);
  local($arglist) = "";
  local($sep);
  
  $sep = "";

  # Put all storage arguments first.
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} =~ m:STORAGESIZE\(([^,]*),\s*(\d+):)
    {
      $arglist .= "$sep"."(int *)(CCTK_STORAGESIZE(xGH, \"$1\",$2)";
      $sep = ",";
    }
  }
  
  # Now deal with the rest of the arguments
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*):;

      if($1 eq "CHAR")
      {
	$arglist .= "$sep"."(char *)((xGH)->data[CCTKARGNUM_$argument])";
	$sep = ",";
      }
      elsif ($1 eq REAL)
      {
	$arglist .= "$sep"."(Double *)((xGH)->data[CCTKARGNUM_$argument])";
	$sep = ",";
      }
      elsif ($1 eq COMPLEX)
      {
	$arglist .= "$sep"."(Complex *)((xGH)->data[CCTKARGNUM_$argument])";
	$sep = ",";
      }
      elsif ($1 eq INTEGER)
      {
	$arglist .= "$sep"."(int *)((xGH)->data[CCTKARGNUM_$argument])";
	$sep = ",";
      }
      else
      {
	print STDERR "Unknown argument type $1\n";
      }
    }
  }

  return $arglist;

}  
1;
