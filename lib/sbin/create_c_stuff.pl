#! /usr/bin/perl
#/*@@
#  @file      create_c_stuff.pl
#  @date      Mon Jan 11 10:53:22 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc
#  @version $Id$ 
#@@*/


#/*@@
#  @routine    CreateParameterBindingFile
#  @date       Wed Jan 20 15:20:23 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the bindings used to link the thorn parameters with the flesh.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateParameterBindingFile
{
  local($prefix, $structure, $n_parameters, @rest) = @_;
  local(%parameter_database);
  local($line,@data);
  local(%parameters);
  local($type, $type_string);
  local(@data);

  %parameters = @rest[0..2*$n_parameters-1];
  %parameter_database = @rest[2*$n_parameters..$#rest];

  # Header Data
  $line = "\#include <stdio.h>";
  push(@data, $line);
  $line = "\#include <stdlib.h>";
  push(@data, $line);
  $line = "\#include <string.h>";
  push(@data, $line);
  $line = "\#include <stdarg.h>";
  push(@data, $line);
  $line = "\#include \"config.h\"";
  push(@data, $line);
  $line = "\#include \"CParameterStructNames.h\"";
  push(@data, $line);
  $line = "\#include \"Misc.h\"";
  $line = "\#include \"ParameterBindings.h\"";
  push(@data, $line);
  push(@data, "");

  # Create the structure

  push(@data,( "struct ", "{"));

  foreach $parameter (order_params(scalar(keys %parameters), %parameters,%parameter_database))
  {
    $type = $parameter_database{"\U$parameters{$parameter} $parameter\E type"};
      
    $type_string = &get_c_type_string($type);

    $line = "  " . $type_string ." " .$parameter . ";";

    push(@data, $line);
  }

  # Some compilers don't like an empty structure.
  if((keys %parameters) == 0)
  {
    push(@data, "  int dummy_parameter;");
  }

  push(@data, "} $structure;");

  push(@data, "");

  # Initialisation subroutine
  push(@data, ("int $prefix"."Initialise(void)", "{"));

  foreach $parameter (keys %parameters)
  {

    push(@data, &set_parameter_default($structure,$parameters{$parameter}, 
				       $parameter, %parameter_database));
    
    push(@data, "");

  }

  push(@data, "  return 0;");
  push(@data, "}");

  push(@data, "");

  # Setting subroutine

  push(@data, ("int $prefix"."Set(const char *param, const char *value)", "{"));
  push(@data, ("  int retval;", "  retval = 1;", ""));


  foreach $parameter (keys %parameters)
  {
    push(@data, &set_parameter_code($structure,$parameters{$parameter}, 
				       $parameter, %parameter_database));
    push(@data, "");

  }    

  push(@data, "  return retval;");

  push(@data, "}");

  push(@data, "");

  # Getting subroutine

  push(@data, ("int $prefix"."Get(const char *param, void **value)", "{"));
  push(@data, ("  int retval;", "  retval = 1;", ""));


  foreach $parameter (keys %parameters)
  {
    push(@data, &get_parameter_code($structure,$parameters{$parameter}, 
				       $parameter, %parameter_database));
    push(@data, "");

  }    

  push(@data, "  return retval;");

  push(@data, "}");

  push(@data, "");

  # Help subroutine

  push(@data, ("int $prefix"."Help(const char *param, const char *format, FILE *file)", "{"));
  push(@data, ("  int retval;", "  retval = 1;", ""));


  foreach $parameter (keys %parameters)
  {
    push(@data, &help_parameter_code($structure,$parameters{$parameter}, 
				       $parameter, %parameter_database));
    push(@data, "");

  }    

  push(@data, "  return retval;");

  push(@data, "}");

  push(@data, "");

  return @data;
}


#/*@@
#  @routine    set_parameter_code
#  @date       Wed Jan 20 15:21:31 1999
#  @author     Tom Goodale
#  @desc 
#  Sets the value of a parameter.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub set_parameter_code
{
  local($structure, $implementation,$parameter, %parameter_database) = @_;
  local($type, $type_string);
  local($line, @lines);
  local($range);
  local($quoted_range);

  $type = $parameter_database{"\U$implementation $parameter\E type"};
  $n_ranges = $parameter_database{"\U$implementation $parameter\E ranges"};

  push(@lines,("  if(CCTK_Equals(param, \"$parameter\"))", "  {"));

  if( $type ne "STRING" && $type ne "SENTENCE" && $type ne "LOGICAL")
  {
    if( $type eq "KEYWORD")
    {
      $line = "    retval = CCTK_SetKeywordInRangeList(\&($structure.$parameter), value, $n_ranges" ;
    }
    elsif($type eq "INTEGER")
    {
      $line = "    retval = CCTK_SetIntInRangeList(\&($structure.$parameter),value, $n_ranges" ;
    }
    elsif($type eq "REAL")
    {
      $line = "    retval = CCTK_SetDoubleInRangeList(\&($structure.$parameter),value, $n_ranges" ;
    }
    for($range=1; $range <= $n_ranges; $range++)
    {
      $quoted_range = $parameter_database{"\U$implementation $parameter\E range $range range"};

      #$quoted_range =~ s:\":\\\":g;
      $quoted_range =~ s:\"::g;
      $quoted_range =~ s:^\s*::;
      $quoted_range =~ s:\s*$::;

      $line .= ",\"".$quoted_range."\"";

    }
    $line .= ");";
  }
  elsif( $type eq "KEYWORD")
  {
    $line = "    retval = CCTK_SetKeyword(\&($structure.$parameter), value);" ;

  }
  elsif( $type eq "STRING" || $type eq "SENTENCE")
  {
    $line = "    retval = CCTK_SetString(\&($structure.$parameter),value);" ;
  }
  elsif( $type eq "LOGICAL")
  {
    $line = "    retval = CCTK_SetLogical(\&($structure.$parameter), value);" ;

  }
  else
  {
    print "Unknown parameter type $type\n";
  }

  push(@lines, ($line, "  }"));
    
  return @lines;
}

    

#/*@@
#  @routine    set_parameter_default
#  @date       Mon Jan 11 15:33:26 1999
#  @author     Tom Goodale
#  @desc 
#  Set the default value of a parameter
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub set_parameter_default
{
  local($structure, $implementation,$parameter, %parameter_database) = @_;
  local($type, $type_string);
  local($line, @lines);
  local($default);
  local($temp_default);

  $default = $parameter_database{"\U$implementation $parameter\E default"};
  $type = $parameter_database{"\U$implementation $parameter\E type"};

  $type_string = &get_c_type_string($type);

  if($type_string eq "char *")
  {
    $line = "  $structure" .".$parameter = malloc(" 
      . (length($default)-1). "\*sizeof(char));";
    push(@lines, $line);

    $line = "  if($structure.$parameter)";
    push(@lines, $line);

    $line = "    strcpy($structure.$parameter, $default);";
    push(@lines, $line);
  }
  elsif($type eq "LOGICAL")
  {
    # Logicals need to be done specially.

    # Strip out any quote marks, and spaces at start and end.
    $temp_default = $default;
    $temp_default =~ s:\"::g;
    $temp_default =~ s:\s*$:: ;
    $temp_default =~ s:^\s*:: ;

    $line = "  CCTK_SetLogical(\&($structure.$parameter),\"$temp_default\");";
    push(@lines, $line);
  }
  else
  {
    $line = "  $structure.$parameter = $default;";
    push(@lines, $line);
  }

  return @lines;
}

#/*@@
#  @routine    get_c_type_string
#  @date       Mon Jan 11 15:33:50 1999
#  @author     Tom Goodale
#  @desc 
#  Returns the correct type string for a parameter
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub get_c_type_string
{
  local($type) = @_;
  local($type_string);


  if($type eq "KEYWORD" ||
     $type eq "STRING"  ||
     $type eq "SENTENCE")
  {
    $type_string = "char *";
  }
  elsif($type eq "LOGICAL" ||
	$type eq "INTEGER")
  {
    $type_string = "int ";
  }
  elsif($type eq "REAL")
  {
    $type_string = "Double ";
  }
  else
  {
    die("Unknown parameter type '$type'");
  }

  return $type_string;

}

#/*@@
#  @routine    GetThornParameterList
#  @date       Wed Jan 20 15:29:40 1999
#  @author     Tom Goodale
#  @desc 
#  Gets a list of all parameters in a particular block in a thorn.
#  Returns a hash table.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub GetThornParameterList
{
  local($thorn, $block, %parameter_database) = @_;
  local(%parameter_list);

  $params = $parameter_database{"\U$thorn $block\E variables"};

  foreach $parameter (split(" ", $params))
  {
    if($parameter =~ m:[^ ]:)
    {
      $parameter_list{$parameter} = $thorn;
    }
  }

  return %parameter_list;
}

sub CreateCStructureParameterHeader
{
  local($prefix, $structure, $n_parameters, @rest) = @_;
  local(%parameter_database);
  local($line,@data);
  local(%parameters);
  local($type, $type_string);
  local(@data);
  local(@definition);

  %parameters = @rest[0..2*$n_parameters-1];
  %parameter_database = @rest[2*$n_parameters..$#rest];

  # Create the structure

  push(@data,( "extern struct ", "{"));

  foreach $parameter (order_params(scalar(keys %parameters), %parameters,%parameter_database))
  {
    $type = $parameter_database{"\U$parameters{$parameter} $parameter\E type"};

    $type_string = &get_c_type_string($type);

    $line = "  ".$type_string ." " .$parameter . ";";

    push(@data, $line);

    $line = $type_string ." " .$parameter . " = $structure.$parameter;";

    push(@definition, $line)
  }

  # Some compilers don't like an empty structure.
  if((keys %parameters) == 0)
  {
    push(@data, "  int dummy_parameter;");
  }

  push(@data, "} $structure;");

  push(@data, "");

  
  push(@data, "#define DECLARE_$structure"."_PARAMS \\");
  
  foreach $line (@definition)
  {
    push(@data, "  $line \\");
  }

  push(@data, "");

  return @data;
}

sub order_params
{
  local($nparams,  @rest) = @_;
  local(%parameters);
  local(%parameter_database);
  local(@float_params) = ();;
  local(@int_params)   = ();
  local(@string_params)= ();

  %parameters = @rest[0..2*$nparams-1];
  %parameter_database = @rest[2*$nparams..$#rest];

  foreach $parameter (keys %parameters)
  {
    $type = $parameter_database{"\U$parameters{$parameter} $parameter\E type"};

    if($type eq "KEYWORD" ||
       $type eq "STRING"  ||
       $type eq "SENTENCE")
    {
      push(@string_params, $parameter);
    }
    elsif($type eq "LOGICAL" ||
    	  $type eq "INTEGER")
    {
      push(@int_params, $parameter);
    }
    elsif($type eq "REAL")
    {
      push(@float_params, $parameter);
    }
    else
    {
      die("Unknown parameter type '$type'");
    }
    
  }
  
  return (@float_params, @string_params, @int_params);
}

sub get_parameter_code
{
  local($structure, $implementation,$parameter, %parameter_database) = @_;
  local($type, $type_string);
  local($line, @lines);
  local($range);
  local($quoted_range);

  $type = $parameter_database{"\U$implementation $parameter\E type"};

  push(@lines,("  if(CCTK_Equals(param, \"$parameter\"))", "  {"));

  if( $type eq "KEYWORD")
  {
    $line  = "    *value = $structure.$parameter;\n" ;
    $line .= "    retval = PARAMETER_KEYWORD;" ;
  }
  elsif( $type eq "STRING")
  {
    $line  = "    *value = $structure.$parameter;\n" ;
    $line .= "    retval = PARAMETER_STRING;" ;
  }
  elsif( $type eq "SENTENCE")
  {
    $line  = "    *value = $structure.$parameter;\n" ;
    $line .= "    retval = PARAMETER_SENTENCE;" ;
  }
  elsif($type eq "INTEGER")
  {
    $line  = "    *value = \&($structure.$parameter);\n" ;
    $line .= "    retval = PARAMETER_INTEGER;" ;
  }
  elsif($type eq "REAL")
  {
    $line  = "    *value = \&($structure.$parameter);\n" ;
    $line .= "    retval = PARAMETER_REAL;" ;
  }
  elsif($type eq "LOGICAL")
  {
    $line  = "    *value = \&($structure.$parameter);\n" ;
    $line .= "    retval = PARAMETER_LOGICAL;" ;
  }
  else
  {
    print "Unknown parameter type $type\n";
  }

  push(@lines, ($line, "  }"));
    
  return @lines;
}

sub help_parameter_code
{
  local($structure, $implementation,$parameter, %parameter_database) = @_;
  local($type, $type_string);
  local($line, @lines);
  local($range);
  local($quoted_range);

  $type = $parameter_database{"\U$implementation $parameter\E type"};

  push(@lines,("  if(CCTK_Equals(param, \"$parameter\"))", "  {"));

  push(@lines, "    printf(\"Help asked for parameter: $implementation\::$parameter.\\n\");");
  push(@lines, ($line, "  }"));
    
  return @lines;
}
  
1;
