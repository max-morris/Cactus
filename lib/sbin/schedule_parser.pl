#! /usr/bin/perl

#%thorns = ("thorn_ADM","thorn_ADM");

#@rfr_file = &create_schedule_code(%thorns);

#print "@rfr_file\n";

#/*@@
#  @routine create_schedule_code
#  @date Fri Jan 8 1999
#  @author Gabrielle Allen
#  @desc
#  Creates the rfr code from the thorn
#  schedule.ccl files
#  @enddesc
#  @calls
#  @calledby
#  @history
#  @endhistory
#@@*/

sub create_schedule_code
{

  local($dir,%thorns) = @_;
  local($thorn);
  local(@rfr_file);
  local(@indata);

  # Loop though each thorn's schedule file
  foreach $thorn (keys %thorns)
  {
     
    $thorn_rfr = "CCTK_".$thorn."_rfr";
    $thorn_startup = "CCTK_".$thorn."_startup";

    open (OUTRFR, ">$dir/Schedule/$thorn_rfr".".c") || die "Cannot open $thorn_rfr".".c";
    open (OUTSTART, ">$dir/Schedule/$thorn_startup".".c") || die "Cannot open $thorn_startup".".c";

    open (PROTO, ">$dir/Schedule/prototypes_$thorn".".h") || die "Cannot open file";

    &write_rfr_header($thorn,$thorn_rfr,OUTRFR);
    &write_startup_header($thorn,$thorn_startup,OUTSTART);

    # Read all the data in the schedule file
    @indata = &read_file("$thorns{$thorn}/schedule.ccl");

    # Parse the data and create rfr and startup subroutines
    @wrapper_files = &parse_schedule_ccl($thorn,"rfr",OUTRFR,PROTO,@indata);
    push (@compile_files,@wrapper_files);
    push (@compile_files,$thorn_rfr);
    push (@compile_files,$thorn_startup);

    &parse_schedule_ccl($thorn,"startup",OUTSTART,PROTO,@indata);

   # The footer for the thorn RFR routine
   print OUTRFR "}\n";
   print OUTSTART "}\n";

   close OUTRFR;
   close OUTSTART;

  }

  return  @compile_files;

}

sub write_rfr_header {

  local($thorn,$routine,$out) = @_;

  # The header for the thorn RFR routine

  print OUTRFR "#define THORN_IS_$thorn\n";
  print OUTRFR "#include \"cctk.h\"\n";
  print OUTRFR "#include \"flesh.h\"\n";
  print OUTRFR "#include \"rfr_constants.h\"\n";
  print OUTRFR "#include \"declare_parameters.h\"\n";
  print OUTRFR "#include \"prototypes_$thorn.h\"\n";
  print OUTRFR "\n";
  print OUTRFR "$routine (void *Cactus_data)\n";
  print OUTRFR "{\n";
  print OUTRFR "  DECLARE_PARAMETERS\n";
  print OUTRFR "\n";

}

sub write_startup_header {

  local($thorn,$routine,$out) = @_;

# The header for the thorn RFR routine

  print OUTSTART "#define THORN_IS_$thorn\n";
  print OUTSTART "#include \"cctk.h\"\n";
  print OUTSTART "#include \"flesh.h\"\n";
  print OUTSTART "#include \"rfr_constants.h\"\n";
  print OUTSTART "#include \"declare_parameters.h\"\n";
  print OUTSTART "\n";
  print OUTSTART "$routine (void *data)\n";
  print OUTSTART "{\n";
  print OUTSTART "  DECLARE_PARAMETERS\n";
  print OUTSTART "\n";


}


sub create_BindingsScheduleRegisterRFR
{
  
  local ($dir,@rfr_routines) = @_;
  local ($rfr_calls,$file,$outfile);

  $outfile = "$dir/Schedule/Cactus_BindingsScheduleRegisterRFR.c";
  open (OUT, ">$outfile") || die "Cannot open $outfile";

  $rfr_calls = "";
  foreach $file (@rfr_routines) {
    $rfr_calls = "$rfr_calls ".$file."(data);\n";
  }

  print OUT <<EOT;

  Cactus_BindingsScheduleRegisterRFR(void *data)
  {
   $rfr_calls
  }
EOT

  return;

}

sub create_BindingsScheduleRegisterSTARTUP
{
  
  local ($dir,@startup_routines) = @_;
  local ($startup_calls,$file,$outfile);

  $outfile = "$dir/Schedule/Cactus_BindingsScheduleRegisterSTARTUP.c";
  open (OUT, ">$outfile") || die "Cannot open $outfile";

  $startup_calls = "";
  foreach $file (@startup_routines) {
    $startup_calls = "$startup_calls ".$file."();\n";
  }

  print OUT <<EOT;

  Cactus_BindingsScheduleRegisterSTARTUP()
  {
   $startup_calls
  }
EOT

  return;

}


#/*@@
#  @routine    parse_schedule_ccl
#  @date       Fri Jan 8 1999
#  @author     Gabrielle Allen
#  @desc 
#  Parse a thorns schedule.ccl file, writes the 
#  corresponding rfr code for each thorn, and
#  writes a wrapper routine for each fortran routine.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub parse_schedule_ccl
{
  local($thorn,$type,$out,$proto,@data) = @_;
  local($line,$line_number,@compile_files);

# Parse the data from the thorns schedule.ccl file
  for ($line_number=0; $line_number<@data; $line_number++)
  {
    $line = @data[$line_number];

    # Parse the entire schedule block
    if ($line =~ m/\s*schedule\s*(.*)\s*at\s*.*/i)
    {
      $wrapper_file = &parse_schedule_block($out,$proto,$thorn,$type,@data);
      push(@compile_files,$wrapper_file);
    }

    # Parse the non-schedule storage line
    elsif ($line =~ m/\s*STORAGE\s*:\s*(.*)/i)
    {
      if ($type eq "rfr")
      {
        @list = split(",",$1);
        foreach $group (@list) 
        {
          print $out "EnableGroupStorage(\"$group\");\n";
        }
      }
    }

    # Parse the non-schedule communication line
    elsif ($line =~ m/\s*COMMUNICATION\s*:\s*(.*)/i)
    {
      if ($type eq "rfr")
      {
        @list = split(",",$1);
        foreach $group (@list) 
        {
          print $out "EnableGroupCommunication(\"$group\");\n";
        }
      }
    }

    # Parse any other lines
    else
    {
      # Any other line is assumed for now to be C
      print $out "$line\n";
    }

  }  

  return @compile_files;

}



#/*@@
#  @routine    find_schedule_block
#  @date       Mon Jan 11 1999
#  @author     Gabrielle Allen
#  @desc 
#  Returns the next block of code in schedule.ccl
#  which schedules a routine, remembering the routine
#  name, the rfr entry point and the routine description
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub find_schedule_block
{
  local(@data) = @_;
  local(@block);
  local($i,$line);

  @block = ();

# Parse the routine name and rfr entry point
  $line = @data[$line_number];
  $line =~ m/\s*schedule\s*(\w*)\s*at\s*(\w*)/i;
  $routine = $1;
  $rfr_entry = $2;

  for ($i=$line_number+1; $i<@data; $i++)
  {
    $line = @data[$i];
    if ($line =~  m/\s*}\s*\"(.*)\"\s*/)
    {
      $line_number = $i;
      $desc = "\"$1\"";
      return ($routine,$rfr_entry,$desc,@block);
    }
    else
    {
      push(@block,($line,"\n"));
    }
  }
}


#/*@@
#  @routine    parse_schedule_block
#  @date       Tue Jan 12 1999
#  @author     Gabrielle Allen
#  @desc 
#  Parse a schedule block of code for keywords
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub parse_schedule_block
{
  local($out,$proto,$thorn,$type,@data)=@_;

  ($routine,$when,$desc,@block) = &find_schedule_block(@data);

  # At the moment can schedule at RFR entry points of at STARTUP
  if ($type eq "startup" && $when eq "STARTUP") {
    &parse_schedule_at_STARTUP($out,$thorn,$routine,$desc,@block);
    return ;
  } elsif ($type eq "rfr" && $when ne "STARTUP") {
    $wrapper_file = &parse_schedule_at_RFR($out,$proto,$thorn,$routine,$when,$desc,@block);
    return $wrapper_file;
  }
}

sub parse_schedule_at_STARTUP {

  local($out,$thorn,$routine,$desc,@block) = @_;

  print $out "  $routine();\n";

  return;

}





sub parse_schedule_at_RFR {

  local($out,$proto,$thorn,$routine,$when,$desc,@block) = @_;
  local($got_it,$i,$line);

# Look for the Language and register routine
  $got_it = 0;
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*LANG\s*:\s*FORTRAN\s*$/i)
    {
      print $out "  rfrRegister(Cactus_data,".$routine."_wrapper,$when,$desc);\n";
      $got_it++;

      # Write the rfr called fortran wrapper routine
      $wrapper_file = &fortran_wrapper($thorn,$routine);
      $routine = "$routine"."_wrapper";
      print $proto "void $wrapper_file(CCTK_CARGUMENTS);\n"; 
   
    }
    elsif ($line =~ m/\s*LANG\s*:\s*C\s*$/i)
    {
      print $proto "void $routine(CCTK_CARGUMENTS);\n"; 
      $got_it++;
    }
  }
  if ($got_it != 1) 
  {
    print "Error in LANG in schedule.ccl $got_it\n";
  }
 
# Look for Storage
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*STORAGE\s*:\s*(.*)\s*$/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {
       print $out "  rfrRegisterStorage(\"$group\",$routine);\n";
      }
    }
  }
 
# Look for Communications
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*COMMUNICATION\s*:\s*(.*)\s*\n/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {
        print $out "  rfrRegisterComm(\"$group\",$routine);\n";
      }
    }
  }

# Look for Triggers
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*TRIGGERS\s*:\s*(.*)\s*\n/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {
        print $out "  rfrRegisterTimer($group,$routine);\n"
      }
    }
  }

  return ($wrapper_file);

}


#/*@@
#  @routine    fortran_wrapper
#  @date       27 Jan 1999
#  @author     Gab Allen
#  @desc 
#  Writes C wrapper for a fortran routine which is called
#  by the RFR. Note that this is done because of the arguement
#  lists. It is assumed that we are in the correct directory 
#  for writing ( build/<config>/build/bindings/Scheduler/ )
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub fortran_wrapper {

  local($thorn,$routine) = @_;
  local($wrapper,$ROUTINE,$file);

  $THORN_C2F = "\U$thorn"."_C2F";
  $THORN_C2F_PROTO = "\U$thorn"."_C2F_PROTO";
  $wrapper = "$routine"."_wrapper";
  $file = "$routine"."_wrapper".".c";

  open(FILE, ">$file") || die("Can't open $file\n");

  print FILE<<EOT;

#define THORN_IS_$thorn
#include \"cctk.h\"
#include \"flesh.h\"
#include \"declare_arguments.h\"

   void FORTRAN_NAME($routine)($THORN_C2F_PROTO);

   $wrapper(CCTK_CARGUMENTS) {

   DECLARE_CCTK_CARGUMENTS
   DECLARE_$THORN_C2F
   INITIALISE_$THORN_C2F

   FORTRAN_NAME($routine)(PASS_$THORN_C2F(GH));

}
EOT

# Return the filename since we have to add it to a makefile
return ($wrapper);

}
 
#/*@@
#  @routine    read_file
#  @date       Wed Sep 16 11:54:38 1998
#  @author     Tom Goodale
#  @desc 
#  Reads a file deleting comments and blank lines. 
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub read_file
{
  local($file) = @_;
  local(@indata);
  
  open(IN, "<$file") || die("Can't open $file\n");
  
  while(<IN>)
  {
    $_ =~ s/\#.*//;
    
    next if(m/^\s+$/);
    
    chop;
    
    push(@indata, $_);
  }
  
  close IN;
  
  return @indata;
}

1;
