#! /usr/bin/perl
#/*@@
#  @file      GridFuncStuff.pl
#  @date      Tue Jan 12 11:07:45 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc 
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

1;
