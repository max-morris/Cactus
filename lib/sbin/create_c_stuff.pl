#! /usr/bin/perl
#/*@@
#  @file      create_c_stuff.pl
#  @date      Mon Jan 11 10:53:22 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc 
#@@*/


#/*@@
#  @routine    create_c_param_init_subroutine
#  @date       Mon Jan 11 14:37:56 1999
#  @author     Tom Goodale
#  @desc 
#  Create a subroutine which initialises an implementation's parameters.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_c_param_init_subroutine
{
  local($implementation,%parameter_database) = @_;
  local(@subroutine);
  local($line, @lines);
  local($block, $parameter);

  $line = "void CCTK_\U$implementation\E_ParamsInit(struct CCTK_PARAMS *cctk_parameters) {";
  push(@subroutine, $line);

  # Deal with variables defined in this thorn. 
  foreach $block ("PUBLIC", "PRIVATE", "PROTECTED")
  {
    $entry = "\U$implementation $block\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      @lines = &set_parameter_default($implementation,$parameter, %parameter_database);
      push(@subroutine, @lines);
    }
  }

  $line = "}";

  push(@subroutine, $line);

  return @subroutine;
}

#/*@@
#  @routine    create_c_parameter_declarations
#  @date       Mon Jan 11 14:39:48 1999
#  @author     Tom Goodale
#  @desc 
#  Create declarations for the parameters used by this thorn.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_c_parameter_declarations
{
  local($implementation,%parameter_database) = @_;
  local(@declarations);
  local($line);
  local($type, $type_string, $friend, $block, $parameter);

  # Deal with variables defined in this thorn. 
  foreach $block ("PUBLIC", "PRIVATE", "PROTECTED")
  {
    $entry = "\U$implementation $block\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = $parameter_database{"\U$implementation $parameter\E type"};
      
      $type_string = &get_c_type_string($type);
      
      $line = "  ". $type_string .$parameter . 
	" = _cctk_params." . "\U$implementation\E.\L$parameter\E;";
      push(@declarations, $line);
    }
  }

  # Deal with friend variables. 
  foreach $friend (split(/ /,$parameter_database{"\U$implementation\E FRIEND implementations"}))
  {
    $other_implementation = "\U$friend\E";
    $entry = "\U$implementation FRIEND $friend\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = $parameter_database{"\U$other_implementation $parameter\E type"};
      
      $type_string = &get_c_type_string($type);
      
      $line = "  ". $type_string .$parameter . 
	" = _cctk_params." . "\U$other_implementation\E.\L$parameter\E;";
      push(@declarations, $line);
    }
  }	  

  foreach $parameter (keys %public_parameters)
  {
    $other_implementation = "\U$public_parameters{$parameter}\E";

    if($other_implementation ne "\U$implementation\E")
    {
      $type = $parameter_database{"\U$other_implementation $parameter\E type"};
      
      $type_string = &get_c_type_string($type);
      
      $line = "  ". $type_string .$parameter . 
	" = _cctk_params." . "\U$other_implementation\E.\L$parameter\E;";
      push(@declarations, $line);
    }
  }

  return @declarations;
}

#/*@@
#  @routine    create_c_parameter_structures
#  @date       Mon Jan 11 15:05:16 1999
#  @author     Tom Goodale
#  @desc 
#  Create the c parameter structures
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_c_parameter_structures
{
  local($n_implementations, @indata) = @_;
  local(@implementations);
  local(%parameter_database);
  local(@structures);
  local(@data);
  local($line, $entry, $thorn, $parameter, $type_string);

  @implementations = @indata[0..$n_implementations-1];
  %parameter_database = @indata[$n_implementations..$#indata];

  # Create types for each implementation
  foreach $implementation (@implementations)
  {
    push(@structures, &create_c_parameter_type_declaration($implementation, %parameter_database));
    push(@strucures,"");
  }

  $line = "struct CCTK_PARAMS {";
  push(@structures, $line);

  foreach $implementation (@implementations)
  {
    $line = "struct CCTK_\U$implementation\E_PARAM_TYPE \U$implementation\E ;";
    push(@structures, $line);
  }

  $line = "} ;";
  push(@structures, $line);
  
  return @structures;
}

#/*@@
#  @routine    create_c_parameter_type_declaration
#  @date       Mon Jan 11 15:32:59 1999
#  @author     Tom Goodale
#  @desc 
#  Create the c parameter type declarations
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_c_parameter_type_declaration
{
  local($implementation,%parameter_database) = @_;
  local(@type_declaration);
  local($line);
  local($type, $type_string, $friend, $block, $parameter);
  
  $line = "struct CCTK_\U$implementation\E_PARAM_TYPE {";
  push(@type_declaration, $line);
 
  # Deal with variables defined in this thorn. 
  foreach $block ("PUBLIC", "PRIVATE", "PROTECTED")
  {
    $entry = "\U$implementation $block\E variables";
    foreach $parameter (split(/ /, $parameter_database{$entry}))
    {
      $type = $parameter_database{"\U$implementation $parameter\E type"};
      
      $type_string = &get_c_type_string($type);
      
      $line = $type_string ." " .$parameter . ";";
      push(@type_declaration, $line);
    }
  }

  $line = "} ;";
  push(@type_declaration, $line);

  return @type_declaration;
}



1;
