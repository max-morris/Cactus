#! /usr/bin/perl
#/*@@
#  @file      create_fortran_stuff.pl
#  @date      Tue Jan 12 09:52:35 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc 
#@@*/


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
  local(@alias_names);

  %parameter_database = @rest[0..(2*$n_param_database)-1];
  %interface_database = @rest[2*$n_param_database..$#rest];

  push(@file, "#define DECLARE_PARAMETERS\\");

  # Generate all public parameters
  %these_parameters = &get_public_parameters(%parameter_database);

  if((keys %these_parameters) > 0)
  {
    @data = &CreateFortranCommonDeclaration("cctk_params_public", 0, scalar(keys %these_parameters), %these_parameters, %parameter_database);

    foreach $line (@data)
    {
      push(@file, "$line&&\\");
    }
  }

  # Generate all protected parameters of this thorn
  %these_parameters = &GetThornParameterList($thorn, "PROTECTED", %parameter_database);

  if((keys %these_parameters > 0))
  {
    $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};

    @data = &CreateFortranCommonDeclaration("cctk_params_$implementation"."_public", 0, scalar(keys %these_parameters), %these_parameters, %parameter_database);

    foreach $line (@data)
    {
      push(@file, "$line&&\\");
    }
  }

  # Generate all private parameters of this thorn
  %these_parameters = &GetThornParameterList($thorn, "PRIVATE", %parameter_database);

  if((keys %these_parameters > 0))
  {
    @data = &CreateFortranCommonDeclaration("cctk_params_$thorn"."_private", 0,scalar(keys %these_parameters), %these_parameters, %parameter_database);

    foreach $line (@data)
    {
      push(@file, "$line&&\\");
    }
  }

  foreach $friend (split(" ",$parameter_database{"\U$thorn\E FRIEND implementations"}))
  {
    $interface_database{"IMPLEMENTATION \U$friend\E THORNS"} =~ m:([^ ]*):;
    
    $friend_thorn = $1;
    
    %these_parameters = &GetThornParameterList($friend_thorn, "PROTECTED", %parameter_database);
    
    @alias_names = ();

    foreach $parameter (keys %these_parameters)
    {
      # Alias the parameter unless it is one we want.
      if(($parameter_database{"\U$thorn FRIEND $friend\E variables"} =~ m:( )*$parameter( )*:) && (length($1) > 0)||length($2)>0||$1 eq $parameter_database{"\U$thorn FRIEND $friend\E variables"})
      {
	push(@alias_names, $parameter);
      }
      else
      {
	push(@alias_names, "CCTKH".scalar(@alias_names));
      }
    }
     
    @data = &CreateFortranCommonDeclaration("cctk_params_$friend_thorn"."_protected", 1, scalar(keys %these_parameters), %these_parameters, @alias_names, %parameter_database);
      
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
  local(@alias_names);
  local($n);

  if($aliases == 0)
  {
    %parameters = @rest[0..2*$n_parameters-1];
    %parameter_database = @rest[2*$n_parameters..$#rest];
  }
  else
  {
    %parameters = @rest[0..2*$n_parameters-1];
    @alias_names = @rest[2*$n_parameters..3*$n_parameters-1];
    %parameter_database = @rest[3*$n_parameters..$#rest];
  }

  # Create the data

  $definition = "COMMON /$common_block/";

  $sepchar = "";

  $n = 0;
  foreach $parameter (keys %parameters)
  {
    $type = $parameter_database{"\U$parameters{$parameter} $parameter\E type"};
      
    $type_string = &get_fortran_type_string($type);

    if($aliases == 0)
    {
      $line = "$type_string $parameter";
    }
    else
    {
      $line = "$type_string $alias_names[$n]";
    }

    push(@data, $line);

    if($aliases == 0)
    {
      $definition .= "$sepchar$parameter";
    }
    else
    {
      $definition .= "$sepchar$alias_names[$n]";
    }


    $sepchar = ",";
    $n++;
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
	$type eq "INTEGER")
  {
    $type_string = "INTEGER ";
  }
  elsif($type eq "REAL")
  {
    $type_string = "REAL ";
  }
  else
  {
    die("Unknown parameter type '$type'");
  }

  return $type_string;

}

1;
