#! /usr/bin/perl
#/*@@
#  @file      CreateFunctionBindings.pl
#  @date      Sat Feb 10 2001
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc
#  @version $Id$ 
#@@*/


#/*@@
#  @routine    CreateFunctionBindings
#  @date       Sat Feb 10 2001
#  @author     Gabrielle Allen
#  @desc 
#  Creates bindings for thorn provided functions
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateFunctionBindings
{
  my($bindings_dir, $rhinterface_db) = @_;
  my($dataout);
  my($function_db);

# Create Function Database
  $function_db = &FunctionDatabase($rhinterface_db);

# Create directories
  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;

  chdir $bindings_dir;

  if(! -d "Functions")
  {
    mkdir("Functions", 0755) || die "Unable to create Functions directory";
  }

  if(! -d "include")
  {
    mkdir("include", 0755) || die "Unable to create include directory";
  }


# Create ThornOverloadables.h
  chdir "include";
  $dataout = &ThornOverloadables($function_db);
  &WriteFile("ThornOverloadables.h",\$dataout);
  chdir $bindings_dir;

# Create DummyThornFunctions.c
  chdir "Functions";
  $dataout = &DummyThornFunctions($function_db);
  &WriteFile("DummyThornFunctions.c",\$dataout);
  chdir $bindings_dir;

# Create FortranThornFunctions.c
  chdir "Functions";
  $dataout = &FortranThornFunctions($function_db);
  &WriteFile("FortranThornFunctions.c",\$dataout);
  chdir $bindings_dir;

# Create THORN_Register.c
  chdir "Functions";
  $registerfiles = "";
  foreach $thorn (split(" ",$rhinterface_db->{"THORNS"}))
  {
    $filename = $thorn."_Register.c";
    $dataout = &RegisterThornFunctions($thorn,$function_db,$rhinterface_db);
    &WriteFile($filename,\$dataout);
    $registerfiles .= " $filename";
  }
  chdir $bindings_dir;

# Create Master registration file RegisterThornFunctions.c
  chdir "Functions";
  $dataout = &RegisterAllFunctions($rhinterface_db);
  &WriteFile("RegisterThornFunctions.c",\$dataout);
  chdir $bindings_dir;



# Create make.code.defn
  chdir "Functions";
  $dataout = "\nSRCS = FortranThornFunctions.c DummyThornFunctions.c RegisterThornFunctions.c $registerfiles\n\n"; 
  &WriteFile("make.code.defn",\$dataout);
  chdir $start_dir;

  return;
}


#/*@@
#  @routine    ThornOverloadables
#  @date       Sat Feb 10
#  @author     Gabrielle Allen
#  @desc 
#  Create include file for thorn function overloads
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub ThornOverloadables
{
  my($function_db) = @_;
  my($dataout,$line,@data);

  # Header Data
  $line = "/*\@\@\n";
  push(@data, $line);
  $line = "  \@header    ThornOverloadables.h\n";
  push(@data, $line);
  $line = "  \@desc\n";
  push(@data, $line);
  $line = "  The overloadable functions from thorns\n";
  push(@data, $line);
  $line = "  See OverloadMacros.h to see how to use these.\n";
  push(@data, $line);
  $line = "  \@enddesc \n";
  push(@data, $line);
  $line = "  \@\@*/\n\n";
  push(@data, $line);
  $line = "\#ifdef OVERLOADABLE_CALL\n";
  push(@data, $line);
  $line = "\#undef OVERLOADABLE_CALL\n";
  push(@data, $line);
  $line = "\#endif\n\n";
  push(@data, $line);
  $line = "\#ifdef OVERLOABLE_PREFIX\n";
  push(@data, $line);
  $line = "\#undef OVERLOADABLE_PREFIX\n";
  push(@data, $line);
  $line = "\#endif\n\n";
  push(@data, $line);
  $line = "\#ifdef OVERLOABLE_DUMMY_PREFIX\n";
  push(@data, $line);
  $line = "\#undef OVERLOADABLE_DUMMY_PREFIX\n";
  push(@data, $line);
  $line = "\#endif\n\n";
  push(@data, $line);
  $line = "\#define OVERLOADABLE_CALL CCTKBindings_\n";
  push(@data, $line);
  $line = "\#define OVERLOADABLE_PREFIX\n";
  push(@data, $line);
  $line = "\#define OVERLOADABLE_DUMMY_PREFIX CCTKBindings_Dummy\n\n";
  push(@data, $line);
  $line = "#ifdef ARGUMENTS\n";
  push(@data, $line);
  $line = "#undef ARGUMENTS\n";
  push(@data, $line);
  $line = "#endif\n\n";
  push(@data, $line);
  $line = "#ifdef RETURN_TYPE\n";
  push(@data, $line);
  $line = "#undef RETURN_TYPE\n";
  push(@data, $line);
  $line = "#endif\n\n";
  push(@data, $line);

  foreach $function (split(" ",$function_db->{"FUNCTIONS"}))
  {
    if ($function !~ m:^\s*$:)
    {
      $line = "\#define ARGUMENTS $function_db->{\"$function CARGS\"}\n";
      push(@data, $line);
      $line = "\#define RETURN_TYPE $function_db->{\"$function RET\"}\n\n";
      push(@data,$line);
      $line = "OVERLOADABLE($function)\n\n";
      push(@data,$line);
      $line = "\#undef ARGUMENTS\n";
      push(@data,$line);
      $line = "\#undef RETURN_TYPE\n\n";
      push(@data,$line);
    }
  }
    
  $line = "#ifdef ARGUMENTS\n";
  push(@data, $line);
  $line = "#undef ARGUMENTS\n";
  push(@data, $line);
  $line = "#endif\n\n";
  push(@data, $line);
  $line = "#ifdef RETURN_TYPE\n";
  push(@data, $line);
  $line = "#undef RETURN_TYPE\n";
  push(@data, $line);
  $line = "#endif\n\n";
  push(@data, $line);
  $line = "\#undef OVERLOADABLE_CALL\n";
  push(@data,$line);
  $line = "\#undef OVERLOADABLE_PREFIX\n";
  push(@data,$line);
  $line = "\#undef OVERLOADABLE_DUMMY_PREFIX\n";
  push(@data,$line);
  
  $dataout = "";
  foreach $line (@data)
  {
    $dataout .= $line;
  }

  return $dataout;
}


#/*@@
#  @routine    RegisterAllFunctions
#  @date       Sun Feb 11 2001
#  @author     Gabrielle Allen
#  @desc 
#  Create file to call all thorn function registration
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub RegisterAllFunctions
{
  my($rhinterface_db) = @_;
  my($dataout,$line,@data);

  $line = "/*\@\@\n";
  push(@data, $line);
  $line = "  \@header    RegisterAllFunctions.c\n";
  push(@data, $line);
  $line = "  \@desc\n";
  push(@data, $line);
  $line = "  Register aliased functions from active thorns\n";
  push(@data, $line);
  $line = "  \@enddesc \n";
  push(@data, $line);
  $line = "  \@\@*/\n\n";
  push(@data, $line);
  $line = "\#include \"cctk_Flesh.h\"\n";
  push(@data, $line);
  $line = "\#include \"cctk_ActiveThorns.h\"\n\n";
  push(@data, $line);

  foreach $thorn (split(" ",$rhinterface_db->{"THORNS"}))
  {
    $line = "int ".$thorn."_RegisterAliases(void);\n";
    push(@data, $line);
  }
  $line = "int CCTKi_BindingsRegisterThornFunctions(void);\n\n";
  push(@data, $line);
  $line = "int CCTKi_BindingsRegisterThornFunctions(void)\n";
  push(@data, $line);
  $line = "{\n";
  push(@data, $line);
  foreach $thorn (split(" ",$rhinterface_db->{"THORNS"}))
  {
    $line = "  if (CCTK_IsThornActive(\"$thorn\"))\n";
    push(@data, $line);
    $line = "  {\n";
    push(@data, $line);
    $line = "    $thorn"."_RegisterAliases();\n";
    push(@data, $line);
    $line = "  }\n";
    push(@data, $line);
  }
  $line = "  return 0;\n";
  push(@data, $line);
  $line = "}\n";
  push(@data, $line);

  $dataout = "";
  foreach $line (@data)
  {
    $dataout .= $line;
  }

  return $dataout;

}

#/*@@
#  @routine    DummyThornFunctions
#  @date       Sat Feb 10
#  @author     Gabrielle Allen
#  @desc 
#  Check contents for ThornOverloadables_h
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub DummyThornFunctions
{
  my($function_db) = @_;
  my($dataout,$line,@data);

  # Header Data
  $line = "/*\@\@\n";
  push(@data, $line);
  $line = "  \@header    DummyThornFunctions.h\n";
  push(@data, $line);
  $line = "  \@desc\n";
  push(@data, $line);
  $line = "  Dummy functions for overloaded thorn functions\n";
  push(@data, $line);
  $line = "  \@enddesc \n";
  push(@data, $line);
  $line = "  \@\@*/\n\n";
  push(@data, $line);
  $line = "\#include \"cctk_Flesh.h\"\n";
  push(@data, $line);
  $line = "\#include \"cctk_WarnLevel.h\"\n\n";
  push(@data, $line);

  foreach $function (split(" ",$function_db->{"FUNCTIONS"}))
  {
    if ($function !~ m:^\s*$:)
      {
	$line = "$function_db->{\"$function RET\"} CCTKBindings_Dummy$function($function_db->{\"$function CARGS\"})\n";
	push(@data, $line);
	$line = "{\n";
	push(@data, $line);
	$line = "  CCTK_Warn(0,__LINE__,__FILE__,\"Bindings\",\n";
	push(@data, $line);
	$line = "            \"CCTKBindings_Dummy$function: Calling thorn function $function which has not been overloaded\");\n";
	push(@data, $line);
	$line = "}\n\n";
	push(@data, $line);
    }      
  }

  $dataout = "";
  foreach $line (@data)
  {
    $dataout .= $line;
  }

  return $dataout;
  
}


#/*@@
#  @routine    RegisterThornFunctions
#  @date       Sun Feb 11 2001
#  @author     Gabrielle Allen
#  @desc 
#  Create contents for files to register aliased functions
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub RegisterThornFunctions
{
  my($thorn,$function_db,$rhinterface_db) = @_;
  my($dataout,$line,@data);

  # Header Data
  $line = "/*\@\@\n";
  push(@data, $line);
  $line = "  \@header    RegisterThornFunctions.h\n";
  push(@data, $line);
  $line = "  \@desc\n";
  push(@data, $line);
  $line = "  Register aliased functions for $thorn\n";
  push(@data, $line);
  $line = "  \@enddesc \n";
  push(@data, $line);
  $line = "  \@\@*/\n\n";
  push(@data, $line);
  $line = "\#include \"cctk_Flesh.h\"\n";
  push(@data, $line);

  foreach $function (split(" ",$rhinterface_db->{"\U$thorn PROVIDES FUNCTION\E"}))
  {
    if ($function !~ m:^\s*$:)
    {
	$line = "$function_db->{\"$function RET\"} $rhinterface_db->{\"\U$thorn PROVIDES FUNCTION\E $function WITH\"}($function_db->{\"$function CARGS\"});\n";
	push(@data, $line);
    }
  }
  $line = "int ".$thorn."_RegisterAliases(void);\n";
  push(@data, $line);
  $line = "int ".$thorn."_RegisterAliases(void)\n";
  push(@data, $line);
  $line = "{\n";
  push(@data, $line);

  foreach $function (split(" ",$rhinterface_db->{"\U$thorn PROVIDES FUNCTION\E"}))
  {
    if ($function !~ m:^\s*$:)
    {
	$line = "CCTKBindings_Overload$function($rhinterface_db->{\"\U$thorn PROVIDES FUNCTION\E $function WITH\"});\n";
	push(@data, $line);
    }
  }
  
  $line = "return 0;\n";
  push(@data, $line);
  $line = "}\n";
  push(@data, $line);
  
  $dataout = "";
  foreach $line (@data)
  {
    $dataout .= $line;
  }

  return ($dataout);
}


#/*@@
#  @routine    FortranThornFunctions
#  @date       Sat Feb 10 2001
#  @author     Gabrielle Allen
#  @desc 
#  Create fortran wrappers for thorn functions
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub FortranThornFunctions
{
  my($function_db) = @_;
  my($dataout,$line,@data);

  # Header Data
  $line = "/*\@\@\n";
  push(@data, $line);
  $line = "  \@header    FortranThornFunctions.h\n";
  push(@data, $line);
  $line = "  \@desc\n";
  push(@data, $line);
  $line = "  Fortran wrappers for overloaded thorn functions\n";
  push(@data, $line);
  $line = "  \@enddesc \n";
  push(@data, $line);
  $line = "  \@\@*/\n\n";
  push(@data, $line);
  $line = "\#include \"cctk_Flesh.h\"\n";
  push(@data, $line);
  $line = "\#include \"cctk_WarnLevel.h\"\n\n";
  push(@data, $line);
  $line = "\#include \"cctk_FunctionAliases.h\"\n\n";
  push(@data, $line);
  $line = "\#include \"cctk_FortranString.h\"\n\n";
  push(@data, $line);

  foreach $function (split(" ",$function_db->{"FUNCTIONS"}))
  {
    if ($function !~ m:^\s*$:)
    {
      $line = "$function_db->{\"$function RET\"} CCTK_FCALL CCTK_FNAME($function)\n";
      push(@data, $line);
      $line = "($function_db->{\"$function WARGS\"}";
      push(@data, $line);

      if ($function_db->{"$function STRINGS"} == 1)
      {
	$line = ", ONE_FORTSTRING_ARG";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 2)
      {
	$line = ", TWO_FORTSTRINGS_ARGS";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 3)
      {
	$line = ", THREE_FORTSTRINGS_ARGS";
	push(@data, $line);
      }

      $line = ")\n";
      push(@data, $line);
      $line = "{\n";
      push(@data, $line);

      if ($function_db->{"$function TYPE"} =~ "FUNC")
      {
	$line = "  $function_db->{\"$function RET\"} cctki_retval;\n";
	push(@data, $line);
      }

      if ($function_db->{"$function STRINGS"} == 1)
      {
	$line = "ONE_FORTSTRING_CREATE(cctki_string1)\n";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 2)
      {
	$line = "TWO_FORTSTRINGS_CREATE(cctki_string1,cctki_string2)\n";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 3)
      {
	$line = "THREE_FORTSTRINGS_CREATE(cctki_string1,cctki_string2,cctki_string3)\n";
	push(@data, $line);
      }

      if ($function_db->{"$function TYPE"} =~ "FUNC")
      {
	$line = "  cctki_retval = \n";
	push(@data, $line);
      }

      $line = "    $function($function_db->{\"$function WCALL\"}";
      push(@data, $line);

      if ($function_db->{"$function STRINGS"} == 1)
      {
	$line = ", cctki_string1";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 2)
      {
	$line = ", cctki_string1, cctki_string2";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 3)
      {
	$line = ", cctki_string1, cctki_string2, cctki_string3";
	push(@data, $line);
      }

      $line = ");\n";
      push(@data, $line);

      if ($function_db->{"$function STRINGS"} == 1)
      {
	$line = "  free(cctki_string1);\n";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 2)
      {
	$line = "  free(cctki_string1);\n  free(cctki_string2);\n";
	push(@data, $line);
      }
      elsif ($function_db->{"$function STRINGS"} == 3)
      {
	$line = "  free(cctki_string1);\n  free(cctki_string2);\n  free(cctki_string3);\n";
	push(@data, $line);
      }

      if ($function_db->{"$function TYPE"} =~ "FUNC")
      {
	$line = "  return cctki_retval; \n";
	push(@data, $line);
      }

      $line = "}\n";
      push(@data, $line);
    }
  }

  $dataout = "";
  foreach $line (@data)
  {
    $dataout .= $line;
  }
  
  return $dataout;
}

#/*@@
#  @routine    FunctionDatabase
#  @date       Wed Dec 06 11.37
#  @author     Gabrielle Allen
#  @desc 
#  Check consistency for Thorn Functions and create database
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub FunctionDatabase
{
  my($rhinterface_db) = @_;

   foreach $thorn (split(" ",$rhinterface_db->{"THORNS"}))
  {
#    print "$thorn\n";
    foreach $function (split(" ",($rhinterface_db->{"\U$thorn FUNCTIONS\E"})))
    {
#      print "  Dealing with $function\n";
      if ($function_db->{"FUNCTIONS"} =~ /$function/ && $function !~ /^\s*$/)
      {
#	print "  Found $function in database\n";
      }
      else
      {
	$inargs = $rhinterface_db->{"\U$thorn FUNCTION\E $function ARGS"};
	$inret  = $rhinterface_db->{"\U$thorn FUNCTION\E $function RET"};
	if ($inret =~ m:^\s*void\s*$:)
	{
	  $function_db->{"$function CARGS"} = "SUB";
	}
	else
	{
	  $function_db->{"$function CARGS"} = "FUNC";
	}

	($nstrings,$fortran,$wrappercall,$wrapperargs,$cargs) = &ParseArguments($inret,$inargs);

	if ($fortran)
	{
	  $function_db->{"$function LANG"} = "FC";
	}
	else
	{
	  $message = "Fortran wrapper not created for alias $function";
	  &CST_error(1,$message,__LINE__,__FILE__);
	  $function_db->{"$function LANG"} = "C";
	}

	$function_db->{"FUNCTIONS"} .= "$function ";
	$function_db->{"$function STRINGS"} = $nstrings;
	$function_db->{"$function CARGS"} = $cargs;
	$function_db->{"$function WARGS"} = $wrapperargs;
	$function_db->{"$function WCALL"} = $wrappercall;
	$function_db->{"$function RET"} = $rhinterface_db->{"\U$thorn FUNCTION\E $function RET"};
      }
    }
  }

  return $function_db;
}

#/*@@
#  @routine    ParseArguments
#  @date       Sun Feb 11 2001
#  @author     Gabrielle Allen
#  @desc 
#  Parse the argument list and create versions for C and Fortran
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub ParseArguments
{
  my($ret,$args) = @_;
  my($number_args);

  print "\n\nInput args is $args\n\n";

  $fwrapperargs = "";
  $fwrappercallargs = "";
  $ccallargs = "";

  $number_args  = split(",",$args);
  $number_strings = 0;
  $fortran = 1;

  foreach $arg (split(",",$args))
  {
    print "arg is $arg\n";
    # last part is the argument name
    $arg =~ m:(.*)\s+([^\s]+)\s*:;
    $name = $2;
    $type = $1;
     
    # treat string differently
    
    if ($type =~ m/char/)
    {
      $number_strings++;
      $ccallargs .= "$type $name, ";
    }
    else
    {
      if ($number_strings)
      {
	$fortran = 0;
      }
      # look for an array
      if ($type =~ m/(.*)(:ARRAY|:\*)\s*/ && $type !~ /const char/i)
	{
	  $ccallargs .= "$1 *$name, ";
	  $fwrapperargs .= "$1 *$name, ";
	  $fwrappercallargs .= "$name, ";
	}
      else
	{
	  $ccallargs .= "$type $name, ";
	  $fwrapperargs .= "$type *$name, ";
	  $fwrappercallargs .= "*$name, ";
	}
    }
  }

  # Remove trailing comma
  $ccallargs =~ s/,\s$//;
  $fwrapperargs =~ s/,\s$//;
  $fwrappercallargs =~ s/,\s$//;

  # Can't do more than three strings for fortran
  if ($number_strings > 3)
  {
    $fortran = 0;
  }

  return ($number_strings,$fortran,$fwrappercallargs,$fwrapperargs,$ccallargs);
}

1;

