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
#  @routine    CreateVariableBindings
#  @date       Thu Jan 28 15:14:20 1999
#  @author     Tom Goodale
#  @desc 
#  Creates all the binding files for the variables.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateVariableBindings
{
  local($bindings_dir, %interface_database) = @_;
  local($thorn, @data);
  local($line, $block, $filelist);

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;
  chdir $bindings_dir;

  # Create the header files
  if(! -d "include")
  {
    mkdir("include", 0755) || die "Unable to create include directory";
  }
  chdir "include";


  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {

    @data = &CreateThornArgumentHeaderFile($thorn, %interface_database);

    open(OUT, ">$thorn"."_arguments.h");

    foreach $line (@data)
    {
      print OUT "$line\n";
    }

    close OUT;
  }

  open(OUT, ">cctk_arguments.h");
    
  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "#ifdef THORN_IS_$thorn\n";
    print OUT "#include \"$thorn"."_arguments.h\"\n";
    print OUT "#define CCTK_FARGUMENTS \U$thorn"."_FARGUMENTS\n";
    print OUT "#define DECLARE_CCTK_FARGUMENTS DECLARE_\U$thorn"."_FARGUMENTS\n";
    print OUT "#define CCTK_CARGUMENTS \U$thorn"."_CARGUMENTS\n";
    print OUT "#define DECLARE_CCTK_CARGUMENTS DECLARE_\U$thorn"."_CARGUMENTS\n";
    print OUT "#endif\n\n";
  }

  close OUT;
      
  chdir "..";

  if(! -d "Variables")
  {
    mkdir("Variables", 0755) || die "Unable to create Variables directory";
  }
  chdir "Variables";

  open (OUT, ">BindingsVariables.c") || die "Cannot open BindingsVariables.c";

  $filelist = "BindingsVariables.c";

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "int CactusBindingsVariables_$thorn"."_Initialise(void);\n";
  }

  print OUT "\n";
 
  print OUT "int CCTKi_BindingsVariablesInitialise(void)\n{\n";

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "  CactusBindingsVariables_$thorn"."_Initialise();\n";
  }
 
  print OUT "  return 0;\n}\n\n";

  close OUT;

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    open(OUT, ">$thorn.c") || die "Cannot create $thorn.c";
  
    print OUT "\#include \"Groups.h\"\n";
#    print OUT "#include \"flesh.h\"\n";
#    print OUT "#include \"StoreVariableData.h\"\n\n";

    print OUT "int CactusBindingsVariables_$thorn"."_Initialise(void)\n{\n";
    foreach $block ("PUBLIC", "PROTECTED", "PRIVATE")
    {
      @data = &CreateThornGroupInitialisers($thorn, $block, %interface_database);

      foreach $line (@data)
      {
	print OUT "$line\n";
      }
    }

    print OUT "  return 0;\n};\n";
    close OUT;

    $filelist .= " $thorn.c";
  }

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  print OUT "SRCS = $filelist\n";

  close OUT;

  chdir $start_dir;
}


#/*@@
#  @routine    GetThornArguments
#  @date       Thu Jan 28 14:31:38 1999
#  @author     Tom Goodale
#  @desc 
#  Gets a list of all the variables available for a thorn in a 
#  particular block.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
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
    @other_imps = split(" ",$interface_database{"IMPLEMENTATION \U$my_imp\E ANCESTORS"});
  }
  elsif($block eq "PROTECTED")
  {
    @other_imps = split(" ", $interface_database{"IMPLEMENTATION \U$my_imp\E FRIENDS"});
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
#  print "Other imps are @other_imps\n";

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
      $ntimelevels = $interface_database{"\U$thorn GROUP $group TIMELEVELS\E"};

      $type = "$vtype";

      if($gtype eq "GF" || $gtype eq "ARRAY")
      {
	$type .= " (";
	$sep = "";
	for($dim =0; $dim < $interface_database{"\U$thorn GROUP $group DIM\E"}; $dim++)
	{
	  $type .= "$sep$group$dim";
	  $sep = ",";
	  if($block eq "PRIVATE")
	  {
	    $arguments{"$group$dim"} = "STORAGESIZE($thorn\::$group, $dim)";
	  }
	  else
	  {
	    $arguments{"$group$dim"} = "STORAGESIZE($imp\::$group, $dim)";
	  }
	}
	$type .= ")";
      }

      if($block eq "PRIVATE")
      {
	$type .= "!$thorn\::$group";
      }
      else
      {
	$type .= "!$imp\::$group";
      }

      $type .="!$ntimelevels";

#      print "Group is $group, resulting type is $type\n";

      foreach $variable (split(" ", $interface_database{"\U$thorn GROUP $group\E"}))
      {
       $arguments{$variable} = $type;
      }
    }
  }

  return %arguments;
}


#/*@@
#  @routine    CreateFortranArgumentDeclarations
#  @date       Thu Jan 28 14:32:57 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the requisite argument list declarations for Fortran.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateFortranArgumentDeclarations
{
  local(%arguments) = @_;
  local($argument);
  local(@declarations) = ();
  local($suffix);

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
    $suffix = "";
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*)!(.*):;

      $ntimelevels = $4;

      for($level = $ntimelevels; $level > 0; $level--)
      {
	# Modify the name for the time level 
	if($ntimelevels == 1)
	{
	  $suffix = "";
	}
	elsif($level == $ntimelevels)
	{
	  $suffix = "_n";
	}
	elsif($level == $ntimelevels-1)
	{
	  $suffix = "";
	}
	else
	{
	  $suffix .= "_p";
	}

	if($1 eq CHAR)
	{
	  push(@declarations, "CCTK_CHAR $argument$suffix$2");
	}
	elsif ($1 eq REAL)
	{
	  push(@declarations, "CCTK_REAL $argument$suffix$2");
	}
	elsif ($1 eq REAL4)
	{
	  push(@declarations, "CCTK_REAL4 $argument$suffix$2");
	}
	elsif ($1 eq REAL8)
	{
	  push(@declarations, "CCTK_REAL8 $argument$suffix$2");
	}
	elsif ($1 eq REAL16)
	{
	  push(@declarations, "CCTK_REAL16 $argument$suffix$2");
	}
	elsif ($1 eq COMPLEX)
	{
	  push(@declarations, "CCTK_COMPLEX $argument$suffix$2");
	}
	elsif ($1 eq INT)
	{
	  push(@declarations, "CCTK_INT $argument$suffix$2");
	}
	elsif ($1 eq INT2)
	{
	  push(@declarations, "CCTK_INT2 $argument$suffix$2");
	}
	elsif ($1 eq INT4)
	{
	  push(@declarations, "CCTK_INT4 $argument$suffix$2");
	}
	elsif ($1 eq INT8)
	{
	  push(@declarations, "CCTK_INT8 $argument$suffix$2");
	}
	else
	{
	  $message = "Unknown argument type \"$1\"";
	  &CST_error(0,$message,__LINE__,__FILE__);
	}
      }
    }
  }

  return @declarations;
    
}


#/*@@
#  @routine    CreateCArgumentDeclarations
#  @date       Jun 29 1999
#  @author     Tom Goodale, Gabrielle Allen
#  @desc 
#  Creates the requisite argument list declarations for C.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateCArgumentDeclarations
{
  local(%arguments) = @_;
  local($argument);
  local(@declarations) = ();
  local($suffix);
  local($imp);

  
  # Now deal with the rest of the arguments
  foreach $argument (sort keys %arguments)
  {
    $suffix = "";
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m\([^ ]*) ?(.*)?!(.*)::(.*)!(.*)\;

      $ntimelevels = $5;

      for($level = $ntimelevels; $level > 0; $level--)
      {
	# Modify the name for the time level 
	if($ntimelevels == 1)
	{
	  $suffix = "";
	}
	elsif($level == $ntimelevels)
	{
	  $suffix = "_n";
	}
	elsif($level == $ntimelevels-1)
	{
	  $suffix = "";
	}
	else
	{
	  $suffix .= "_p";
	}

	if($1 eq "CHAR")
	{
	  push(@declarations, "CCTK_CHAR *$argument$suffix=(CCTK_CHAR *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq REAL)
	{
	  push(@declarations, "CCTK_REAL *$argument$suffix=(CCTK_REAL *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq REAL4)
	{
	  push(@declarations, "CCTK_REAL4 *$argument$suffix=(CCTK_REAL4 *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq REAL8)
	{
	  push(@declarations, "CCTK_REAL8 *$argument$suffix=(CCTK_REAL8 *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq REAL16)
	{
	  push(@declarations, "CCTK_REAL16 *$argument$suffix=(CCTK_REAL16 *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq COMPLEX)
	{
	  push(@declarations, "CCTK_COMPLEX *$argument$suffix=(CCTK_COMPLEX *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq INT)
	{
	  push(@declarations, "CCTK_INT *$argument$suffix=(CCTK_INT *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq INT2)
	{
	  push(@declarations, "CCTK_INT2 *$argument$suffix=(CCTK_INT2 *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq INT4)
	{
	  push(@declarations, "CCTK_INT4 *$argument$suffix=(CCTK_INT4 *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	elsif ($1 eq INT8)
	{
	  push(@declarations, "CCTK_INT8 *$argument$suffix=(CCTK_INT8 *)(cctkGH->data[CCTK_VarIndex(\"$3::$argument\")][0]);");
	}
	else
	{
	  $message = "Unknown argument type $1";
	  &CST_error(0,$message,__LINE__,__FILE__);
	}
      }
    }
  }

  return @declarations;
    
}



#/*@@
#  @routine    CreateFortranArgumentList
#  @date       Thu Jan 28 14:33:50 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the argument list a Fortran subroutine sees.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

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
      $suffix = "";
      if($arguments{$argument} !~ m:STORAGESIZE:)
      {
	$arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*)!(.*):;
	
	$ntimelevels = $4;
	
	for($level = $ntimelevels; $level > 0; $level--)
	{
	  # Modify the name for the time level 
	  if($ntimelevels == 1)
	  {
	    $suffix = "";
	  }
	  elsif($level == $ntimelevels)
	  {
	    $suffix = "_n";
	  }
	  elsif($level == $ntimelevels-1)
	  {
	    $suffix = "";
	  }
	  else
	  {
	    $suffix .= "_p";
	  }
	  
	  $argumentlist .= "$sep$argument$suffix";
	  $sep = ",";
	}
      }
    }
  }
  return $argumentlist;
}

#/*@@
#  @routine    CreateCArgumentStatics
#  @date       Thu Jan 28 14:33:50 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the declarations of static variables used to speed up
#  construction of arguments to pass to Fortran.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

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


#/*@@
#  @routine    CreateCArgumentInitialisers
#  @date       Thu Jan 28 14:33:50 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the code to initialise the statics.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateCArgumentInitialisers
{
  local(%arguments) = @_;
  local($argument);
  local(@initialisers) = ();

  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m,([^ ]*) ?(.*)?!(.*)\::(.*)!(.*),;

      push(@initialisers, "if(CCTKARGNUM_$argument == -1) CCTKARGNUM_$argument = CCTK_VarIndex(\"$3::$argument\")");
    }
  }

  return @initialisers;
}

#/*@@
#  @routine    CreateCArgumentPrototype
#  @date       Thu Jan 28 14:36:25 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the prototype needed to call a Fortran function from C.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

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

      $suffix = "";
      if($arguments{$argument} !~ m:STORAGESIZE:)
      {
	$arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*)!(.*):;
	
	$ntimelevels = $4;
	
	for($level = $ntimelevels; $level > 0; $level--)
	{
	  if($1 eq "CHAR")
	  {
	    $prototype .="$sep". "char *";
	    $sep = ",";
	  }
	  elsif ($1 eq REAL)
	  {
	    $prototype .="$sep". "CCTK_REAL *";
	    $sep = ",";	
	  }
	  elsif ($1 eq REAL4)
	  {
	    $prototype .="$sep". "CCTK_REAL4 *";
	    $sep = ",";	
	  }
	  elsif ($1 eq REAL8)
	  {
	    $prototype .="$sep". "CCTK_REAL8 *";
	    $sep = ",";	
	  }
	  elsif ($1 eq REAL16)
	  {
	    $prototype .="$sep". "CCTK_REAL16 *";
	    $sep = ",";	
	  }
	  elsif ($1 eq COMPLEX)
	  {
	    $prototype .="$sep". "CCTK_COMPLEX *";
	    $sep = ",";
	  }
	  elsif ($1 eq INT)
	  {
	    $prototype .="$sep". "CCTK_INT *";
	    $sep = ",";
	  }
	  elsif ($1 eq INT2)
	  {
	    $prototype .="$sep". "CCTK_INT2 *";
	    $sep = ",";
	  }
	  elsif ($1 eq INT4)
	  {
	    $prototype .="$sep". "CCTK_INT4 *";
	    $sep = ",";
	  }
	  elsif ($1 eq INT8)
	  {
	    $prototype .="$sep". "CCTK_INT8 *";
	    $sep = ",";
	  }
	  else
	  {
	    $message = "Unknown argument type $1";
	    &CST_error(0,$message,__LINE__,__FILE__);
	  }
	}
      }
    }
  }
  return $prototype;
}



#/*@@
#  @routine    CreateCArgumentList
#  @date       Thu Jan 28 14:37:07 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the argument list used to call a Fortran function from C.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

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
      $arglist .= "$sep"."(int *)(CCTK_STORAGESIZE(xGH, \"$1\",$2))";
      $sep = ",";
    }
  }
  
  # Now deal with the rest of the arguments
  foreach $argument (sort keys %arguments)
  {
    if($arguments{$argument} !~ m:STORAGESIZE:)
    {
      $arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*):;

      $suffix = "";
      if($arguments{$argument} !~ m:STORAGESIZE:)
      {
	$arguments{$argument} =~ m:([^ ]*) ?(.*)?!(.*)!(.*):;
	
	$ntimelevels = $4;
	
	for($level = $ntimelevels; $level > 0; $level--)
	{
	  if($1 eq "CHAR")
	  {
	    $arglist .= "$sep"."(CCTK_CHAR *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq REAL)
	  {
	    $arglist .= "$sep"."(CCTK_REAL *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq REAL)
	  {
	    $arglist .= "$sep"."(CCTK_REAL *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq REAL4)
	  {
	    $arglist .= "$sep"."(CCTK_REAL4 *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq REAL8)
	  {
	    $arglist .= "$sep"."(CCTK_REAL8 *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq REAL16)
	  {
	    $arglist .= "$sep"."(CCTK_REAL16 *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq COMPLEX)
	  {
	    $arglist .= "$sep"."(CCTK_COMPLEX *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq INT)
	  {
	    $arglist .= "$sep"."(CCTK_INT *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq INT2)
	  {
	    $arglist .= "$sep"."(CCTK_INT2 *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq INT4)
	  {
	    $arglist .= "$sep"."(CCTK_INT4 *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  elsif ($1 eq INT8)
	  {
	    $arglist .= "$sep"."(CCTK_INT8 *)((xGH)->data[CCTKARGNUM_$argument][$level-1])";
	    $sep = ",";
	  }
	  else
	  {
	    $message = "Unknown argument type $1";
	    &CST_error(0,$message,__LINE__,__FILE__);
	  }
	}
      }
    }
  }
  return $arglist;
}  

#/*@@
#  @routine    CreateThornArgumentHeaderFile
#  @date       Thu Jan 28 14:37:58 1999
#  @author     Tom Goodale
#  @desc 
#  Creates all the argument list stuff necessary to call Fortran from C
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateThornArgumentHeaderFile
{
  local($thorn, %interface_database) = @_;
  local($line);
  local(@returndata) = ();
  local(%hasvars);

  # Create the basic thorn block definitions 

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    
    %data = &GetThornArguments($thorn, $block, %interface_database);

    # Remember if there actually are any arguments here.
    $hasvars{$block} = 1 if(keys %data > 0) ;

    # Do the fortran definitions
    push(@returndata, "#ifdef FCODE");

    # Create the fortran argument declarations

    @data = &CreateFortranArgumentDeclarations(%data);
   
    push(@returndata, "#define \UDECLARE_$thorn"."_$block"."_FARGUMENTS \\");

    foreach $line (@data)
    {
      push(@returndata, "$line&&\\");
    }

    push(@returndata, ("",""));

    # Create the fortran argument list 

    push(@returndata, "#define \U$thorn"."_$block"."_FARGUMENTS \\");

    push(@returndata, &CreateFortranArgumentList(%data));

    push(@returndata, ("",""));

    push(@returndata, "#endif /*FCODE*/");

    push(@returndata, ("",""));


    ##########################################################

    # Do the C definitions
    push(@returndata, "#ifdef CCODE");

    # Create the C argument declarations

    @data = &CreateCArgumentDeclarations(%data);
   
    push(@returndata, "#define \UDECLARE_$thorn"."_$block"."_CARGUMENTS \\");

    foreach $line (@data)
    {
      push(@returndata, "$line \\");
    }

    push(@returndata, ("",""));

    # Create the C argument variable number statics

    push(@returndata, "#define \UDECLARE_$thorn"."_$block"."_C2F \\");

    @data = &CreateCArgumentStatics(%data);
    foreach $line (@data)
    {
      push(@returndata, "$line; \\");
    }
    push(@returndata, ("",""));


    # Create the C argument variable number statics initialisers
    push(@returndata, "#define \UINITIALISE_$thorn"."_$block"."_C2F \\");
    @data = &CreateCArgumentInitialisers(%data);
    foreach $line (@data)
    {
      push(@returndata,"$line; \\");
    }

    push(@returndata, ("",""));

    # Create the C argument prototypes
    push(@returndata, "#define \U$thorn"."_$block"."_C2F_PROTO \\");

    push(@returndata, &CreateCArgumentPrototype(%data));

    push(@returndata, ("",""));

    # Create the C argument list
    push(@returndata, "#define \UPASS_$thorn"."_$block"."_C2F(xGH) \\");

    push(@returndata, &CreateCArgumentList(%data));

    push(@returndata, ("",""));

    push(@returndata, "#endif /*CCODE*/");

    push(@returndata, ("",""));

  }

  ################################################################


  # Create the final thorn argument macros

  # Do the Fortran argument lists
  push(@returndata, "#ifdef FCODE");

  $sep = ",";

  push(@returndata, "#define \U$thorn"."_FARGUMENTS _CCTK_FARGUMENTS\\");
  $sep = ",";

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    if($hasvars{$block})
    {
      push(@returndata, "$sep"."\U$thorn"."_$block"."_FARGUMENTS\\");
      $sep = ",";
    }
  }

  push(@returndata, ("",""));

  # Do the fortran declarations
  push(@returndata, "#define \UDECLARE_$thorn"."_FARGUMENTS _DECLARE_CCTK_FARGUMENTS \\");

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    if($hasvars{$block})
    {
      push(@returndata, "DECLARE_\U$thorn"."_$block"."_FARGUMENTS \\");
    }
  }

  push(@returndata, ("",""));

  push(@returndata, "#endif /*FCODE*/");
  
  push(@returndata, ("",""));

  

  push(@returndata, "#ifdef CCODE");

  # Don't need C arguments

  # Do the C declarations
  push(@returndata, "#define \UDECLARE_$thorn"."_CARGUMENTS _DECLARE_CCTK_CARGUMENTS \\");

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    if($hasvars{$block})
    {
      push(@returndata, "DECLARE_\U$thorn"."_$block"."_CARGUMENTS \\");
    }
  }

  push(@returndata, ("",""));

  push(@returndata, "#endif /*CCODE*/");
  
  push(@returndata, ("",""));


  ################################################
  
  # Do the C definitions
  push(@returndata, "#ifdef CCODE");

  $sep = "";

  # Argument prototypes
  push(@returndata, "#define \U$thorn"."_C2F_PROTO _CCTK_C2F_PROTO\\");
  $sep = ",";

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    if($hasvars{$block})
    {
      push(@returndata, "$sep"."\U$thorn"."_$block"."_C2F_PROTO\\");
    }
  }

  push(@returndata, ("",""));

  # Argument lists
  $sep = "";

  push(@returndata, "#define PASS_\U$thorn"."_C2F(xGH) _PASS_CCTK_C2F(xGH)\\");
  $sep = ",";

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    if($hasvars{$block})
    {
      push(@returndata, "$sep"."PASS_\U$thorn"."_$block"."_C2F(xGH)\\");
    }
  }

  push(@returndata, ("",""));

  # Declare statics

  push(@returndata, "#define DECLARE_\U$thorn"."_C2F _DECLARE_CCTK_C2F \\");

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    if($hasvars{$block})
    {
      push(@returndata, "DECLARE_\U$thorn"."_$block"."_C2F \\");
    }
  }

  push(@returndata, ("",""));

  # Initialise statics

  push(@returndata, "#define INITIALISE_\U$thorn"."_C2F _INITIALISE_CCTK_C2F \\");

  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
  {
    if($hasvars{$block})
    {
      push(@returndata, "INITIALISE_\U$thorn"."_$block"."_C2F \\");
    }
  }

  push(@returndata, ("",""));

  # Dummy C declarations

  push(@returndata, "#define \U$thorn"."_CARGUMENTS cGH *cctkGH ");

#  push(@returndata, "#define \UDECLARE_$thorn"."_CARGUMENTS \\");

#  foreach $block ("PRIVATE", "PROTECTED", "PUBLIC")
#  {
#    if($hasvars{$block})
#    {
#	push(@returndata, "\UDECLARE_$thorn_$block"."_CARGUMENTS \\");
#    }
#  }

  push(@returndata, ("",""));

  push(@returndata, "#endif /*CCODE*/");
  
  push(@returndata, ("",""));
  
  return @returndata;
}



#/*@@
#  @routine    CreateThornGroupInitialisers
#  @date       Thu Jan 28 14:38:56 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the calls used to setup groups for a particular thorn block.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateThornGroupInitialisers  
{
  local($thorn, $block, %interface_database) = @_;
  local($imp);
  local($group, @variables);
  local($line);
  local(@definitions);


  $imp = $interface_database{"\U$thorn\E IMPLEMENTS"};

  foreach $group (split(" ", $interface_database{"\U$thorn $block GROUPS"}))
  {
    @variables = split(" ", $interface_database{"\U$thorn GROUP $group\E"});

    $line  = "  CCTK_CreateGroup(\"\U$group\",\"$thorn\",\"$imp\",\n" 
           . "                   \"" . $interface_database{"\U$thorn GROUP $group\E GTYPE"} . "\",\n"
	   . "                   \"" . $interface_database{"\U$thorn GROUP $group\E VTYPE"} . "\",\n"
	   . "                   \"" . $block . "\",\n"
	   . "                   " . $interface_database{"\U$thorn GROUP $group\E DIM"} . ",\n"
	   . "                   " . $interface_database{"\U$thorn GROUP $group\E TIMELEVELS"} . ",\n"
           . "                   ". scalar(@variables);
    foreach $variable (@variables)
    {
      $line .= ",\n                   \"$variable\"";
    }

    $line  .= ");\n\n";

    push(@definitions, $line);
  }

  return @definitions;
  
}

1;
