#/*@@
#  @file      CreateParameterBindings.pl
#  @date      Sun Jul 25 00:52:36 1999
#  @author    Tom Goodale
#  @desc 
#  Parameter binding stuff
#  @enddesc 
#@@*/

#/*@@
#  @routine    CreateParameterBindings
#  @date       Thu Jan 28 15:27:16 1999
#  @author     Tom Goodale
#  @desc 
#  Create the bindings used for the parameters.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/

sub CreateParameterBindings
{
  local($bindings_dir, $n_param_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local($start_dir);
  local($line);
  local(%these_parameters);
  local($implementation, $thorn);
  local($files);
  local(%routines);
  local($structure, %structures);
  local(%header_files);

  %parameter_database = @rest[0..(2*$n_param_database)-1];
  %interface_database = @rest[2*$n_param_database..$#rest];

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;

  chdir $bindings_dir;

  if(! -d "Parameters")
  {
    mkdir("Parameters", 0755) || die "Unable to create Parameters directory";
  }

  if(! -d "include")
  {
    mkdir("include", 0755) || die "Unable to create include directory";
  }

  chdir "Parameters";


  # Generate all global parameters
  %these_parameters = &get_global_parameters(%parameter_database);

  @data = &CreateParameterBindingFile("CCTK_BindingsParametersGlobal", "GLOBAL_PARAMETER_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

  open (OUT, ">Global.c") || die "Cannot open Global.c";

  foreach $line (@data)
  {
    print OUT "$line\n";
  }

  close OUT;

  $files = "Global.c";
  $structures{"GLOBAL_PARAMETER_STRUCT"} = "cctk_params_global";

  # Generate the global data header file

  chdir "..";
  chdir "include";

  @data = &CreateCStructureParameterHeader("CCTK_BindingsParametersGlobal", "GLOBAL_PARAMETER_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

  open (OUT, ">ParameterCGlobal.h") || die "Cannot open ParameterCGlobal.h";

  foreach $line (@data)
  {
    print OUT "$line\n";
  }

  close OUT;

  $header_files{"GLOBAL"} = "ParameterCGlobal.h";

  chdir "..";
  chdir "Parameters";

  # Generate all restricted parameters
  foreach $implementation (split(" ",$interface_database{"IMPLEMENTATIONS"}))
  {
    $interface_database{"IMPLEMENTATION \U$implementation\E THORNS"} =~ m:([^ ]+):;

    $thorn = $1;

    %these_parameters = &GetThornParameterList($thorn, "RESTRICTED", %parameter_database);

    if((keys %these_parameters > 0))
    {
      @data = &CreateParameterBindingFile("CCTK_BindingsParameters$implementation"."_restricted", "RESTRICTED_\U$implementation\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

      open (OUT, ">\U$implementation\E". "_restricted.c") || die "Cannot open \U$implementation\E"."_restricted.c";
      
      foreach $line (@data)
      {
	print OUT "$line\n";
      }
    
      close OUT;

      $files .= " \U$implementation\E". "_restricted.c";
      $routines{"CCTK_BindingsParameters$implementation"."_restricted"} = "$implementation";
      $structures{"RESTRICTED_\U$implementation\E_STRUCT"} = "$implementation"."rest";

      # Generate the data header file
      
      chdir "..";
      chdir "include";

      @data = &CreateCStructureParameterHeader("CCTK_BindingsParameters$implementation"."_restricted", "RESTRICTED_\U$implementation\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);
      
      open (OUT, ">ParameterCRestricted\U$implementation\E".".h") || die "Cannot open ParameterCRestricted\U$implementation\E".".h";

      foreach $line (@data)
      {
	print OUT "$line\n";
      }

      close OUT;

      $header_files{"\U$implementation\E RESTRICTED"} = "ParameterCRestricted\U$implementation\E".".h";

      chdir "..";
      chdir "Parameters";

    }
  }

  # Generate all private parameters
  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    %these_parameters = &GetThornParameterList($thorn, "PRIVATE", %parameter_database);

    if((keys %these_parameters > 0))
    {
      @data = &CreateParameterBindingFile("CCTK_BindingsParameters$thorn"."_private", "PRIVATE_\U$thorn\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

      open (OUT, ">\U$thorn\E"."_private.c") || die "Cannot open \U$thorn\E"."_private.c";

      foreach $line (@data)
      {
	print OUT "$line\n";
      }
    
      close OUT;

      $files .= " \U$thorn\E". "_private.c";
      $routines{"CCTK_BindingsParameters$thorn"."_private"} = "$thorn";

      # Generate the data header file
      
      chdir "..";
      chdir "include";

      @data = &CreateCStructureParameterHeader("CCTK_BindingsParameters$thorn"."_private", "PRIVATE_\U$thorn\E_STRUCT", scalar(keys %these_parameters), %these_parameters, %parameter_database);

      $structures{"PRIVATE_\U$thorn\E_STRUCT"} = "$thorn"."priv";
      
      open (OUT, ">ParameterCPrivate\U$thorn\E".".h") || die "Cannot open ParameterCPrivate\U$thorn\E".".h";

      foreach $line (@data)
      {
	print OUT "$line\n";
      }

      close OUT;

      $header_files{"\U$thorn\E PRIVATE"} = "ParameterCPrivate\U$thorn\E".".h";

      chdir "..";
      chdir "Parameters";

    }
  }

  open (OUT, ">BindingsParameters.c") || die "Cannot open BindingsParameters.c";

  print OUT  <<EOT;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "cctk_Misc.h"
#include "cctk_WarnLevel.h"

EOT

  foreach $routine ((keys %routines), "CCTK_BindingsParametersGlobal")
  {
    print OUT "int $routine"."Initialise(void);\n";
#    print OUT "int $routine"."Set(const char *param, const char *value);\n";
#    print OUT "int $routine"."Get(const char *param, void **data);\n";
    print OUT "int $routine"."Help(const char *param, const char *format, FILE *file);\n";
  }
 
print OUT <<EOT;

int CCTKi_BindingsParametersInitialise(void)
{

EOT

#  foreach $routine (keys %routines, "CCTK_BindingsParametersGlobal")
#  {
#    print OUT "  $routine"."Initialise();\n";
#  }

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "  CCTKi_BindingsCreate$thorn"."Parameters();\n\n";
  }

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "  CCTKi_Bindings$thorn"."ParameterExtensions();\n\n";
  }

  print OUT <<EOT;

  return 0;
}
 
EOT

#int CCTKi_BindingsParameterSet(const char *identifier, const char *value)
#{
#  int retval = 1;
#  int temp_retval;
#  char *implementation = NULL;
#  char *param_name = NULL;

#  Util_SplitString(&implementation, &param_name, identifier, "::");

#  if(!implementation)
#  {
#    retval = CCTK_BindingsParametersGlobalSet(identifier, value);
#  }
#  else
#  { 

#    if(CCTK_IsThornActive(implementation) ||
#       CCTK_IsImplementationActive(implementation))
#    {
#EOT

#  foreach $routine (keys %routines, "CCTK_BindingsParametersGlobal")
#  {
#
#    print OUT <<EOT;
#
#    if(CCTK_Equals(implementation, \"$routines{$routine}\"))
#    {
#EOT
#      print OUT "        temp_retval =  $routine"."Set(param_name, value);";
#
#    print OUT <<EOT;
# 
#        if(!temp_retval) 
#        {
#          retval = 0;
#        }  
#      }
#EOT
#  }
 
#  print OUT <<EOT;
#    }
#    else
#    {
#       char *message = malloc( (200+strlen(param_name)+strlen(implementation))*sizeof(char) );
#       sprintf(message, "Can't set %s - %s is not active", param_name, implementation);
#       CCTK_Warn(0,__LINE__,__FILE__,"CactusBindings",message);
#       free(message);
#       retval = -2;
#    }
#  }  
# 
#  free(implementation);
#  free(param_name);
#  return retval;
#}
 
#int CCTKi_BindingsParameterGet(const char *identifier, void **value)
#{   
# int retval = 1;
#  int temp_retval;
#  char *implementation = NULL;
#  char *param_name = NULL;
#
#  Util_SplitString(&implementation, &param_name, identifier, "::");
#
#  if(!implementation)
#  {
#    retval = CCTK_BindingsParametersGlobalGet(identifier, value);
#  }
#  else
#  { 
#EOT
#
#  foreach $routine (keys %routines, "CCTK_BindingsParametersGlobal")
#  {
#
#    print OUT <<EOT;
#
#    if(CCTK_Equals(implementation, \"$routines{$routine}\"))
#    {
#EOT
#      print OUT "      temp_retval =  $routine"."Get(param_name, value);";
#
#    print OUT <<EOT;
# 
#      if(!temp_retval) 
#      {
#        retval = 0;
#      }
#    }
#EOT
#  }
# 
#  print OUT <<EOT;
#  }
# 
#  free(implementation);
#  free(param_name);
#  return retval;
#}

  print OUT <<EOT;
int CCTKi_BindingsParameterHelp(const char *identifier, const char *format, FILE *file) 
{   
  int retval = 1;
  int temp_retval;
  char *implementation = NULL;
  char *param_name = NULL;

  if(! identifier )
  {
    retval = CCTK_BindingsParametersGlobalHelp(identifier, format, file);

EOT

  foreach $routine (keys %routines, "CCTK_BindingsParametersGlobal")
  {

    print OUT "      temp_retval =  $routine"."Help(param_name, format, file);";

    print OUT <<EOT;
 
    if(!temp_retval) 
    {
      retval = 0;
    }
EOT
  }
 
  print OUT <<EOT;

    return retval;
  }

  Util_SplitString(&implementation, &param_name, identifier, "::");

  if(!implementation)
  {
    retval = CCTK_BindingsParametersGlobalHelp(identifier, format, file);
  }
  else
  { 
EOT

  foreach $routine (keys %routines, "CCTK_BindingsParametersGlobal")
  {

    print OUT <<EOT;

      if(CCTK_Equals(implementation, \"$routines{$routine}\"))
      {
EOT
      print OUT "      temp_retval =  $routine"."Help(param_name, format, file);";

    print OUT <<EOT;
 
      if(!temp_retval) 
      {
        retval = 0;
      }
    }
EOT
  }
 
  print OUT <<EOT;
  }
 
  free(implementation);
  free(param_name);
  return retval;
}

EOT
  
  close OUT;

  $newfilelist = NewParamStuff($n_param_database, @rest);

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  print OUT "SRCS = BindingsParameters.c $files $newfilelist\n";

  close OUT;

  # Create the appropriate thorn parameter headers

  chdir "..";
  chdir "include";

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {

    @data = &CreateFortranThornParameterBindings($thorn, $n_param_database, @rest);

    open(OUT, ">\U$thorn\E"."_FParameters.h") || die "Cannot open \U$thorn\E"."_FParameters.h";

    foreach $line (@data)
    {
      print OUT "$line\n";
    }

    close OUT;

    open(OUT, ">\U$thorn\E"."_CParameters.h") || die "Cannot open \U$thorn\E"."_CParameters.h";

    $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};

    print OUT <<EOT;
 
\#ifndef _\U$thorn\E_PARAMETERS_H_
 
\#define _\U$thorn\E_PARAMETERS_H_

EOT

  if($header_files{"GLOBAL"})
  {
    print OUT "#include \"". $header_files{"GLOBAL"} ."\"\n";
  }
 
  if($header_files{"\U$implementation\E RESTRICTED"})
  {
    print OUT "#include \"". $header_files{"\U$implementation\E RESTRICTED"}."\"\n";
  }

  if($header_files{"\U$thorn\E PRIVATE"})
  {
    print OUT "#include \"".$header_files{"\U$thorn\E PRIVATE"}."\"\n";
  }
 
  print OUT "\n";

    @data = ();
    foreach $friend (split(" ",$parameter_database{"\U$thorn\E SHARES implementations"}))
    {
      $friend_implementation = $interface_database{"\U$friend\E IMPLEMENTS"};

      print OUT "#include \"ParameterCRestricted\U$friend\E.h\"\n";

      $interface_database{"IMPLEMENTATION \U$friend\E THORNS"} =~ m:([^ ]*):;
 
      $friend_thorn = $1;

      foreach $parameter (split(" ",$parameter_database{"\U$thorn SHARES $friend\E variables"}))
      {
	$type = $parameter_database{"\U$friend_thorn $parameter\E type"};

	$type_string = &get_c_type_string($type);

	$line = "  ".$type_string ." " .$parameter . " = RESTRICTED_\U$friend\E_STRUCT.$parameter;";

	push(@data, $line);

      }
    }

    print OUT "#define DECLARE_CCTK_PARAMETERS \\\n";

    $decl =  "DECLARE_GLOBAL_PARAMETER_STRUCT_PARAMS";
    if($header_files{"GLOBAL"})
    {
      print OUT "$decl \\\n";
    }
 
    $decl = "DECLARE_RESTRICTED_\U$implementation\E_STRUCT_PARAMS";
    if($header_files{"\U$implementation\E RESTRICTED"})
    {
      print OUT "$decl \\\n";
    }

    $decl = "DECLARE_PRIVATE_\U$thorn\E_STRUCT_PARAMS";
    if($header_files{"\U$thorn\E PRIVATE"})
    {
      print OUT "$decl \\\n";
    }

    foreach $line (@data)
    {
      print OUT $line . "\\\n";
    }

    print OUT "\n";

    print OUT "#endif\n";

    close OUT;
  }   

  open(OUT, "| perl $cctk_home/lib/sbin/c_file_processor.pl $top/config-data > CParameterStructNames.h") || die "Cannot create CParameterStructNames.h by running c_file_procesor.pl";
  foreach $structure (keys %structures)
  {
    print OUT "#define $structure FORTRAN_COMMON_NAME($structures{$structure})\n";
  }

  print OUT "\n";

  close OUT;
    
  open(OUT, ">CParameters.h") || die "Cannot open CParameters.h";

  print OUT "#include \"CParameterStructNames.h\"\n\n";

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "#ifdef THORN_IS_$thorn\n";
    print OUT "#include \"\U$thorn\E"."_CParameters.h\"\n";
    print OUT "#endif\n\n";
  }

  close OUT;

  open(OUT, ">FParameters.h") || die "Cannot open FParameters.h";

  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    print OUT "#ifdef THORN_IS_$thorn\n";
    print OUT "#include \"\U$thorn\E"."_FParameters.h\"\n";
    print OUT "#endif\n\n";
  }

  close OUT;

  open(OUT, ">cctk_parameters.h") || die "Cannot open cctk_parameters.h";

  print OUT "#ifdef CCODE\n";
  print OUT "#include \"CParameters.h\"\n";
  print OUT "#endif\n\n";


  print OUT "#ifdef FCODE\n";
  print OUT "#include \"FParameters.h\"\n";
  print OUT "#endif\n\n";

  close OUT;

  chdir $start_dir;


}

sub NewParamStuff
{
  local($n_param_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local($line);
  local(%these_parameters);
  local($implementation, $thorn);
  local($files);
  local(%routines);
  local($structure, %structures);
  local(%header_files);
  local($thorn, $block); 
  local($filelist);
  local(@creationdata);
  local(@extensiondata);
  local(@data);

  %parameter_database = @rest[0..(2*$n_param_database)-1];
  %interface_database = @rest[2*$n_param_database..$#rest];


  foreach $thorn (split(" ",$interface_database{"THORNS"}))
  {
    $imp = $interface_database{"\U$thorn\E IMPLEMENTS"};

    push(@data, "#include <stdarg.h>");
    push(@data, "");
    push(@data, "#include \"config.h\"");
    push(@data, "#include \"ParameterBindings.h\"");

    push(@data, "#include \"CParameterStructNames.h\"");

    foreach $block ("GLOBAL", "RESTRICTED", "PRIVATE")
    {
      %these_parameters = &GetThornParameterList($thorn, $block, %parameter_database);

      if((keys %these_parameters > 0))
      {
	if($block eq "GLOBAL")
	{
	  push(@data, "#include \"ParameterCGlobal.h\"");
	}
	elsif($block eq "RESTRICTED")
	{
	  push(@data, "#include \"ParameterCRestricted\U$imp\E.h\"");
	}
	elsif($block eq "PRIVATE")
	{
	  push(@data, "#include \"ParameterCPrivate\U$thorn\E.h\"");
	}
	else
	{
	  die "Internal error";
	}

#	print "Generating $block parameters for $thorn, providing $imp\n";
	push(@creationdata,&CreateParameterRegistrationStuff($block, $thorn, $imp, scalar(keys %these_parameters), %these_parameters, %parameter_database));
      }
    }


    # Now the parameter extensions
#    print $parameter_database{"\U$thorn\E SHARES implementations"} . "\n";

    foreach $block (split(" ",$parameter_database{"\U$thorn\E SHARES implementations"}))
    {

      push(@data, "#include \"ParameterCRestricted\U$block\E.h\"");

#      print "Generating $block extension from $thorn\n";
      push(@extensiondata,&CreateParameterExtensionStuff($block, $thorn, %parameter_database));

    }

    push(@data, "");
    push(@data, "int CCTKi_BindingsCreate$thorn"."Parameters(void)");
    push(@data, "{");

    push(@data, @creationdata);

    push(@data, "}");

    push(@data, "");
    push(@data, "int CCTKi_Bindings$thorn"."ParameterExtensions(void)");
    push(@data, "{");

    push(@data, @extensiondata);

    push(@data, "}");

    open (OUT, ">Create$thorn"."Parameters.c");

    foreach $line (@data)
    {
      print OUT "$line\n";
    }
    
    close OUT;

    @data=();
    @creationdata=();
    @extensiondata=();

    $filelist .= " Create$thorn"."Parameters.c";
  }

  return $filelist;
}

sub CreateParameterRegistrationStuff
{
  local($block, $thorn, $imp, $n_params, @rest) = @_;
  local(%these_parameters);
  local(%parameter_database);
  local(@data);
  local($line);
  local($structure, $type, $n_ranges);

  %these_parameters = @rest[0..(2*$n_params)-1];
  %parameter_database = @rest[2*$n_params..$#rest];

  if($block eq "GLOBAL")
  {
    $structure="GLOBAL_PARAMETER_STRUCT";
  }
  elsif($block eq "RESTRICTED")
  {
    $structure="RESTRICTED_\U$imp\E_STRUCT";
  }
  elsif($block eq "PRIVATE")
  {
    $structure = "PRIVATE_\U$thorn\E_STRUCT";
  }
  else
  {
    die "Internal error";
  }

#  print "Thorn is $thorn\n";
#  print "Structure is $structure\n";

  foreach $parameter (sort keys %these_parameters)
  {

#    print "This param is $parameter\n";

    $type = $parameter_database{"\U$thorn $parameter\E type"};
    
#    print "Type is $type\n";
    
    $n_ranges = $parameter_database{"\U$thorn $parameter\E ranges"};
    
#    print "N_ranges is $n_ranges\n";
    
    $quoted_default = $parameter_database{"\U$thorn $parameter\E default"};
 
#    $quoted_default =~ s:\"::g;  The database now strips all unescaped quotes.

    $line="  ParameterCreate(\"$parameter\", /* The parameter name */\n".
          "                  \"$thorn\",     /* The thorn          */\n". 
          "                  \"$type\"       /* The parameter type */,\n".
          "                  \"$block\",     /* The scoping block  */\n".
          "                  0,              /* Is it steerable ?  */\n".
          "                  " . $parameter_database{"\U$thorn $parameter\E description"} . ", /* The description */\n" .
          "                  \"" . $quoted_default . "\",  /* The default value */\n" .
          "                  &($structure.$parameter),   /* The actual data pointer */\n".
          "                  $n_ranges       /* How many allowed ranges it has */";
    
    for($range=1; $range <= $n_ranges; $range++)
    {
      $quoted_range = $parameter_database{"\U$thorn $parameter\E range $range range"};
      $range_description = $parameter_database{"\U$thorn $parameter\E range $range description"};

      if($range_description !~ m:\":)
      {
	$range_description = "\"$range_description\"";
      }

      $range_description =~ s:,$::;

      #$quoted_range =~ s:\":\\\":g;
      $quoted_range =~ s:\"::g;
      $quoted_range =~ s:^\s*::;
      $quoted_range =~ s:\s*$::;

      $line .= ",\n                  \"".$quoted_range."\", $range_description";

    }

    $line .=");\n";

    push(@data, $line);
  }


  return @data;
}

sub CreateParameterExtensionStuff
{
  local($block, $thorn, %parameter_database) = @_;
  local(@data);
  local($line);
  local($structure, $type, $n_ranges, $range, $quoted_range, $range_description);
 
#  print "Extending $block from $thorn\n";

  foreach $parameter (split(" ",$parameter_database{"\U$thorn\E SHARES \U$block\E variables"}))
  {
    $n_ranges = $parameter_database{"\U$thorn $parameter\E ranges"};

    for($range=1; $range <= $n_ranges; $range++)
    {
      $quoted_range = $parameter_database{"\U$thorn $parameter\E range $range range"};
      $range_description = $parameter_database{"\U$thorn $parameter\E range $range description"};

      if($range_description !~ m:\":)
      {
	$range_description = "\"$range_description\"";
      }

      #$quoted_range =~ s:\":\\\":g;
      $quoted_range =~ s:\"::g;
      $quoted_range =~ s:^\s*::;
      $quoted_range =~ s:\s*$::;

      push(@data, "  ParameterAddRange(\"$block\",");
      push(@data, "                    \"$parameter\",");
      push(@data, "                    \"$thorn\",");
      push(@data, "                    \"$quoted_range\",");
      push(@data, "                    $range_description);");
      push(@data, "");


#      print "Adding \"$quoted_range\" to $parameter\n";


    }

  }

  return @data;
}

1;
