#/*@@
#  @file      CreateScheduleBindings.pl
#  @date      Sun Jul 25 00:53:43 1999
#  @author    Tom Goodale
#  @desc 
#  Schedule bindings stuff
#  @enddesc 
#@@*/


#/*@@
#  @routine    CreateScheduleBindings
#  @date       Thu Jan 28 15:27:16 1999
#  @author     Tom Goodale
#  @desc 
#  Create the bindings used for the scheduler.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub CreateScheduleBindings
{
  local($bindings_dir,$n_thorns,@rest) = @_;
  local(%thorns);
  local(%interface_database);
  local($wrapper, $rfr, $startup, %schedule_data);

  %thorns = @rest[0..2*$n_thorns-1];
  %interface_database = @rest[2*$n_thorns..$#rest];

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;
  chdir $bindings_dir;

  if(! -d "Schedule")
  {
    mkdir("Schedule", 0755) || die "Unable to create Schedule directory";
  }
  chdir "Schedule";

  # Parse the schedule.ccl files
  if ($CST_debug)
  {
    print "DEBUG: Parsing schedule.ccl files\n";
  }
  ($wrapper,$rfr,$startup, %schedule_data) = &create_schedule_code($bindings_dir,$n_thorns,%thorns,%interface_database);

  @rfr_files = split(" ",$rfr);

  # Write the contents of BindingsScheduleRegisterRFR.c
  if ($CST_debug)
  {
    print "DEBUG: Creating RFR registration files\n";
  }
  &create_RegisterRFR($bindings_dir,scalar(@rfr_files), @rfr_files, %schedule_data); 

  # Write the contents of BindingsScheduleRegisterSTARTUP.c
  
  if ($CST_debug)
  {
    print "DEBUG: Creating STARTUP registration files\n";
  }
  &create_RegisterSTARTUP($bindings_dir,split(" ",$startup)); 

  open (OUT, ">BindingsSchedule.c") || die "Cannot open BindingsSchedule.c";

  print OUT  <<EOT;
#include <stdio.h>
 
  int CCTKi_BindingsScheduleInitialise(void)
  {
    return 0;
  }

  int CCTKi_BindingsScheduleRegister(const char *type, void *data)
  {
    
    if (CCTK_Equals(type,"STARTUP"))
    {
      Cactus_RegisterSTARTUP();
    } 
    else if (CCTK_Equals(type,"RFRINIT")) 
    {
      Cactus_RegisterRFR(data);
    } else {
      printf ("Unknown type in CCTKi_BindingsScheduleRegister");
    }

    return 0;
  }
 
EOT

  close OUT;

  open (OUT, ">make.code.defn") || die "Cannot open make.code.defn";

  $files = "";
  foreach $file (split(" ",$rfr),split(" ",$startup),split(" ",$wrapper)) {
    if($file)
    {
      $files = "$files ".$file.".c";
    }
  }

  print OUT "SRCS = BindingsSchedule.c Cactus_RegisterSTARTUP.c Cactus_RegisterRFR.c $files\n";

  close OUT;

  chdir $start_dir;
}

1;
