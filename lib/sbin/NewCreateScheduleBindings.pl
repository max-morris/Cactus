#/*@@
#  @file      NewCreateScheduleBindings.pl
#  @date      Thu Sep 16 23:30:21 1999
#  @author    Tom Goodale
#  @desc 
#  New schedule stuff.  Should be renamed !!!
#  @enddesc 
#  @version $Header$
#@@*/

sub NewCreateScheduleBindings
{
  local($bindings_dir, $n_param_database, $n_interface_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local(%schedule_database);
  local($start_dir);
  local($thorn);
  local($implementation);
  local($buffer, $prototypes);
  local($block_buffer, $block_prototype);

  # Extract the parameter,interface, and schedule databases from the arguments.
  %parameter_database = @rest[0..2*$n_param_database-1];
  %interface_database = @rest[2*$n_param_database..2*($n_param_database+$n_interface_database)-1];
  %schedule_database = @rest[2*($n_param_database+$n_interface_database)..$#rest];
  
  foreach $thorn (sort split(" ", $interface_database{"THORNS"}))
  {
    $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};

    $buffer = $schedule_database{"\U$thorn\E FILE"};

    for($block = 0 ; $block < $schedule_database{"\U$thorn\E N_BLOCKS"}; $block++)
    {
      ($block_buffer, $block_prototype) = &ScheduleBlock($thorn, $implementation, $block, %schedule_database);
      $buffer =~ s:\@BLOCK\@$block:$block_buffer:;
      $prototypes .= "$block_prototype";
    }
    
    print "---------------------------------\n";
    print "$thorn -> $implementation\n";
    print "Prototypes:\n";
    print "$prototypes\n";
    print "Buffer:\n";
    print "$buffer\n";
    print "---------------------------------\n";

  }

}  

sub ScheduleBlock
{
  local($thorn, $implementation, $block, %schedule_database) = @_;
  local($buffer, $prototype);

  return ($buffer, $prototype);
}
