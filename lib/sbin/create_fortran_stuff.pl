#! /usr/bin/perl
#/*@@
#  @file      create_fortran_stuff.pl
#  @date      Tue Jan 12 09:52:35 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc 
#@@*/

$BindingsAliasNum = 0;

sub CreateFortranThornParameterBindings
{
  local($thorn, $n_param_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local($line);
  local(%these_parameters);
  local($implementation);
  local(@data);
  local(@file);
  local(%alias_names);

  %parameter_database = @rest[0..(2*$n_param_database)-1];
  %interface_database = @rest[2*$n_param_database..$#rest];

  push(@file, "#define DECLARE_CCTK_PARAMETERS \\");

  # Generate all global parameters
  %these_parameters = &get_global_parameters(%parameter_database);

  if((keys %these_parameters) > 0)
  {
    @data = &CreateFortranCommonDeclaration("cctk_params_global", 0, scalar(keys %these_parameters), %these_parameters, %parameter_database);

    foreach $line (@data)
    {
      push(@file, "$line&&\\");
    }
  }

  # Generate all restricted parameters of this thorn
  %these_parameters = &GetThornParameterList($thorn, "RESTRICTED", %parameter_database);

  if((keys %these_parameters > 0))
  {
    $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};
    
    @data = &CreateFortranCommonDeclaration("$implementation"."rest", 0, scalar(keys %these_parameters), %these_parameters, %parameter_database);

    foreach $line (@data)
    {
      push(@file, "$line&&\\");
    }
  }

  # Generate all private parameters of this thorn
  %these_parameters = &GetThornParameterList($thorn, "PRIVATE", %parameter_database);

  if((keys %these_parameters > 0))
  {
    @data = &CreateFortranCommonDeclaration("$thorn"."priv", 0,scalar(keys %these_parameters), %these_parameters, %parameter_database);

    foreach $line (@data)
    {
      push(@file, "$line&&\\");
    }
  }

  # Parameters from friends
  foreach $friend (split(" ",$parameter_database{"\U$thorn\E SHARES implementations"}))
  {

    # Determine which thorn provides this friend implementation
    $interface_database{"IMPLEMENTATION \U$friend\E THORNS"} =~ m:([^ ]*):;
    
    $friend_thorn = $1;
    
    %these_parameters = &GetThornParameterList($friend_thorn, "RESTRICTED", %parameter_database);
    
    %alias_names = ();

    foreach $parameter (sort(keys %these_parameters))
    {
      # Alias the parameter unless it is one we want.
      if(($parameter_database{"\U$thorn SHARES $friend\E variables"} =~ m:( )*$parameter( )*:) && (length($1) > 0)||length($2)>0||$1 eq $parameter_database{"\U$thorn SHARES $friend\E variables"})
      {
	$alias_names{$parameter} = "$parameter";
      }
      else
      {
	$alias_names{$parameter} = "CCTKH".$BindingAliasNum;
	$BindingAliasNum++;
      }
    }

    @data = &CreateFortranCommonDeclaration("$friend"."rest", 1, scalar(keys %these_parameters), %these_parameters, %alias_names, %parameter_database);
      
    foreach $line (@data)
    {
      push(@file, "$line&&\\");
    }

    
  }

  push(@file, ("",""));
  
  return (@file);
}

sub CreateFortranCommonDeclaration
{
  local($common_block, $aliases, $n_parameters, @rest) = @_;
  local(%parameter_database);
  local($line,@data);
  local(%parameters);
  local($type, $type_string);
  local($definition);
  local(%alias_names);

  if($aliases == 0)
  {
    %parameters = @rest[0..2*$n_parameters-1];
    %alias_names = ();
    %parameter_database = @rest[2*$n_parameters..$#rest];
  }
  else
  {
    %parameters = @rest[0..2*$n_parameters-1];
    %alias_names = @rest[2*$n_parameters..4*$n_parameters-1];
    %parameter_database = @rest[4*$n_parameters..$#rest];
  }

  # Create the data

  $definition = "COMMON /$common_block/";

  $sepchar = "";

  foreach $parameter (&order_params(scalar(keys %parameters), %parameters,%parameter_database))
  {
    $type = $parameter_database{"\U$parameters{$parameter} $parameter\E type"};
      
    $type_string = &get_fortran_type_string($type);

    if($aliases == 0)
    {
      $line = "$type_string $parameter";
    }
    else
    {
      $line = "$type_string $alias_names{$parameter}";
    }

    push(@data, $line);

    if($aliases == 0)
    {
      $definition .= "$sepchar$parameter";
    }
    else
    {
      $definition .= "$sepchar$alias_names{$parameter}";
    }


    $sepchar = ",";
  }

  push(@data, $definition);

  return @data;
}
  

sub get_fortran_type_string
{
  local($type) = @_;
  local($type_string);


  if($type eq "KEYWORD" ||
     $type eq "STRING"  ||
     $type eq "SENTENCE")
  {
    $type_string = "CCTK_STRING ";
  } 
  elsif($type eq "LOGICAL" ||
	$type eq "INT")
  {
    $type_string = "CCTK_INT";
  }
  elsif($type eq "REAL")
  {
    $type_string = "CCTK_REAL ";
  }
  else
  {
    die("Unknown parameter type '$type'");
  }

  return $type_string;

}

1;
