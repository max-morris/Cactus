#! /usr/bin/perl

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

  local($dir,$n_thorns,@rest) = @_;
  local($thorn);
  local(@rfr_file);
  local(@indata,$implementation);
  local($schedule_wrappers, $wrapper_files);
  local(%schedule_ordering);
  local(@schedule_data);
  local(@retschedule_data);
    
  %thorns = @rest[0..2*$n_thorns-1];
  %interface_database = @rest[2*$n_thorns..$#rest];

  # Loop though each thorn's schedule file
  foreach $thorn (keys %thorns)
  {
    if ($CST_debug)
    {
	print "DEBUG:   --> $thorn\n";
    }

    $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};

#   Private groups and variables for thorn
    $privategroups = $interface_database{"\U$thorn\E PRIVATE GROUPS"};
    $privatevars = "";
    foreach $var (split(" ", $privategroups))
    {
      $privatevars .= " $interface_database{\"\U$thorn GROUP $var\"} ";
    }

    $thorn_rfr = $thorn."_rfr";
    $thorn_startup = $thorn."_startup";

    open (OUTRFR, ">$dir/Schedule/$thorn_rfr".".c") || die "Cannot open $thorn_rfr".".c";
    open (OUTSTART, ">$dir/Schedule/$thorn_startup".".c") || die "Cannot open $thorn_startup".".c";

    $header = &write_rfr_header($thorn,$thorn_rfr);
    print OUTRFR $header;
    &write_startup_header($thorn,$thorn_startup,OUTSTART);

    # Read all the data in the schedule file
    @indata = &read_file("$thorns{$thorn}/schedule.ccl");

    # Parse the data and create rfr and startup subroutines
    ($proto,$out,$wrapper_files, @retscheduledata) = &parse_schedule_ccl(1,$privategroups,$privatevars,$thorn,$implementation,"rfr",@indata);
    print OUTRFR $proto;
    print OUTRFR $out; 
    $rfr_files .= " $thorn_rfr";
    $startup_files .= " $thorn_startup";
    push(@schedule_data, @retscheduledata);

    ($proto,$out,$schedule_wrappers) = &parse_schedule_ccl(2,$privategroups,$privatevars,$thorn,$implementation,"startup",@indata);
    print OUTSTART $proto;
    print OUTSTART $out; 

   # The footer for the thorn RFR routine
   print OUTRFR "  }\n}\n";
   print OUTSTART " }\n}\n";

   close OUTRFR;
   close OUTSTART;

  }

  %schedule_ordering = @schedule_data;

  $schedule_ordering{":THORNS:"} = join(" ", keys %thorns);

  $schedule_ordering{":ROUTINES:"} = "";

  foreach $thorn (keys %thorns)
  {
    foreach $routine (split(" ", $schedule_ordering{"\U$thorn"}))
    {
      $schedule_ordering{":ROUTINES:"} .= "$routine ";
    }
  }

  return  ($wrapper_files,$rfr_files,$startup_files, %schedule_ordering);

}

sub write_rfr_header {

  local($thorn,$routine) = @_;
  local($header);

  # The header for the thorn RFR routine

  $header  = "#define THORN_IS_$thorn\n";
  $header .= "#include <stdio.h>\n";
  $header .= "#include \"cctk.h\"\n";
  $header .= "#include \"cctk_Flesh.h\"\n";
  $header .= "#include \"cctk_Comm.h\"\n";
  $header .= "/* FIXME - remove when ActiveThorns does not need this */\n";
  $header .= "#include \"SKBinTree.h\"\n\n";
  $header .= "#include \"cctk_ActiveThorns.h\"\n";
  $header .= "#include \"cctk_Groups.h\"\n";
  $header .= "#include \"cctk_GroupsOnGH.h\"\n";
  $header .= "#include \"rfrConstants.h\"\n";
  $header .= "#include \"cctk_parameters.h\"\n";
  $header .= "#include \"cctk_arguments.h\"\n";
  $header .= "#include \"cctk_WarnLevel.h\"\n";
  $header .= "\n";
  $header .= "void $routine (cGH *GH)\n";
  $header .= "{\n";
  $header .= "  DECLARE_CCTK_PARAMETERS\n";
  $header .= "  int index,varfirst,varlast,varindex;\n\n";
  $header .= "\n";
  $header .= "  if(CCTK_IsThornActive(\"$thorn\"))\n";
  $header .= "  {\n\n";

  return $header;

}

sub write_startup_header {

  local($thorn,$routine,$out) = @_;

# The header for the thorn STARTUP routine
  print OUTSTART "#define THORN_IS_$thorn\n";
  print OUTSTART "#include <stdio.h>\n";
  print OUTSTART "#include \"cctk.h\"\n";
  print OUTSTART "#include \"cctk_Flesh.h\"\n";
  print OUTSTART "#include \"cctk_WarnLevel.h\"\n";
  print OUTSTART "/* FIXME - remove when ActiveThorns does not need this */\n";
  print OUTSTART "#include \"SKBinTree.h\"\n\n";
  print OUTSTART "#include \"cctk_ActiveThorns.h\"\n";
  print OUTSTART "#include \"rfrConstants.h\"\n";
  print OUTSTART "#include \"cctk_parameters.h\"\n";
  print OUTSTART "\n";
  print OUTSTART "void $routine ()\n";
  print OUTSTART "{\n";
  print OUTSTART "  DECLARE_CCTK_PARAMETERS\n";
  print OUTSTART "\n";
  print OUTSTART  "  if(CCTK_IsThornActive(\"$thorn\"))\n";
  print OUTSTART "  {\n\n";


}


sub create_RegisterRFR
{
  
  local ($dir,$n_rfrfiles, @rest) = @_;
  local ($rfr_calls, $rfr_order_calls, $rfr_order_prototypes, $outfile);
  local(@sorted_routines);
  local($routine);
  local($order, %order);

  if($n_rfrfiles == 0)
  {
    @rfr_files = ();
    %schedule_data = @rest;
  }
  else
  {
    @rfr_files = @rest[0..$n_rfrfiles-1];
    %schedule_data = @rest[$n_rfrfiles..$#rest];
  }

  $outfile = "$dir/Schedule/Cactus_RegisterRFR.c";
  open (OUT, ">$outfile") || die "Cannot open $outfile";

  $rfr_calls = "";
  $rfr_order_calls = "";
  $rfr_order_prototypes = "";

  # Sort the rfr routines
  if ($CST_debug)
  {
    print "DEBUG: Ordering RFR routines\n";
  }
  @sorted_routines = &OrderList("Scheduling error", ":ROUTINES:", %schedule_data);
  $order = 1;

  foreach $routine (@sorted_routines)
  {

    if ($CST_debug)
    {
	print "DEBUG:   --> $routine\n";
    }

    if($routine)
    {
      $rfr_order_prototypes .= "void $routine(cGH *);\n";
      $rfr_order_calls .= "  rfrRegisterOrderNumber($routine, $order);\n";
      $order++;
    }
  }

  foreach $file (@rfr_files) 
  {
    $rfr_calls = "$rfr_calls ".$file."(data);\n";
  }

  print OUT <<EOT;

#include "cctk_Flesh.h"

$rfr_order_prototypes
void Cactus_RegisterRFR(void *data)
{
$rfr_order_calls
$rfr_calls
}
EOT

  return;

}

sub create_RegisterSTARTUP
{
  
  local ($dir,@startup_routines) = @_;
  local ($startup_calls,$file,$outfile);

  $outfile = "$dir/Schedule/Cactus_RegisterSTARTUP.c";
  open (OUT, ">$outfile") || die "Cannot open $outfile";

  $startup_calls = "";
  foreach $file (@startup_routines) {
    $startup_calls = "$startup_calls ".$file."();\n";
  }

  print OUT <<EOT;

  int Cactus_RegisterSTARTUP()
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
  local($number,$privategroups,$privatevars,$thorn,$implementation,$type,@data) = @_;
  local($proto,$out,$line,$line_number,@compile_files);
  local(%schedule_ordering);
  local($routine);

# Parse the data from the thorns schedule.ccl file
  for ($line_number=0; $line_number<@data; $line_number++)
  {
    $line = @data[$line_number];

    @options = ();

    # Parse the entire schedule block
    if ($line =~ m/\s*schedule\s*(.*)\s*at\s*(.*)/i)
    {
      $routine = $1;
      @options = split(" ", $2);

      ($wrapper_file,$proto_block,$out_block, $routine) = &parse_schedule_block($number,$privategroups,$privatevars,$thorn,$implementation,$type,@data);

      $proto .= "$proto_block"; 
      $out .= "$out_block";
      $compile_files .= " $wrapper_file";

      if(@options && $options[0] !~ m:STARTUP:i)
      {
	$schedule_ordering{"\U$thorn"} .= " $routine";

	for($option = 0; $option < $#options; $option++)
	{
	  if($options[$option] =~ m:\bBEFORE\b:i)
	  {
	    $schedule_ordering{"\U$routine BEFORE"} .= " $options[$option+1]";
	    $option++;
	  }
	  elsif($options[$option] =~ m:\bAFTER\b:i)
	  {
	    $schedule_ordering{"\U$routine AFTER"} .= " $options[$option+1]";
	    $option++;
	  }
	}
      }
    }

    # Parse the non-schedule storage line
    elsif ($line =~ m/\s*STOR[^\:]*:\s*(.*)\s*$/i)
    {
      if ($type eq "rfr")
      {
        @list = split(",",$1);
        foreach $group (@list) 
        {
	  # Strip of any spaces 
	  $group =~ s/ //g;

	  $this_imp = $implementation;
	  $this_group = $group;
	  if ($group =~ /(.*)::(.*)/)
       	  {
	    $this_imp = $1;
	    $this_group = $2;
	  }

	  if ($privategroups =~ /\b$this_group\b/)
	  {
	    $use_imp = $thorn;
	  }
	  else
	  {
	    $use_imp = $this_imp;
	  }

          $out .= "CCTK_EnableGroupStorage(GH,\"$use_imp\::$this_group\");\n";
        }
      }
    }

    # Parse the non-schedule communication line
    elsif ($line =~ m/\s*COMM[^\:]*:\s*(.*)/i)
    {
      if ($type eq "rfr")
      {
        @list = split(",",$2);
        foreach $group (@list) 
        {
	  # Strip of any spaces 
	  $group =~ s/ //g;

	  $this_imp = $implementation;
	  $this_group = $group;
	  if ($this_group =~ /(.*)::(.*)/)
	  {
	    $this_imp = $1;
	    $this_group = $2;
	  }
          $out .= "CCTK_EnableGroupComm(GH,\"$this_imp\::$this_group\");\n";
        }
      }
    }

    # Parse any other lines
    else
    {
      # Any other line is assumed for now to be C
      $out .= "$line\n";
    }

  }  

  return ($proto,$out,$compile_files, %schedule_ordering);

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
  local($thorn,$number,@data) = @_;
  local(@block);
  local($i,$line);

  @block = ();

# Parse the routine name and rfr entry point
  $line = @data[$line_number];
  $line =~ m/\s*schedule\s*(\w*)\s*at\s*(\w*)/i;
  $routine = $1;
  $rfr_entry = $2;
 
  # Allow entry point with or without the CCTK_
  if ($rfr_entry !~ /CCTK_/)
  {
    $rfr_entry = "CCTK_".$rfr_entry;
  }

  for ($i=$line_number+1; $i<@data; $i++)
  {
    $line = @data[$i];
    if ($line =~  m/\}/)
    {
      $line =~  m/\s*\}\s*\"(.*)\"\s*/;
      $line_number = $i;
      if($1)
      {	
	$desc = "\"$thorn: $1\"";
      }
      else
      {
	  if ($number == 1)
          {
	      print STDERR "No description listed for routine $routine registered at $rfr_entry\n";
	  }
	  $desc = "\"Please write a description of what this routine does.\"";
      }
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
  local($number,$privategroups,$privatevars,$thorn,$implementation,$type,@data)=@_;
  local($proto,$out);
  local($wrapper_file, $proto, $out);

  $wrapper_file = "";
  $proto = "";
  $out = "";

  ($routine,$when,$desc,@block) = &find_schedule_block($thorn,$number,@data);

  # At the moment can schedule at RFR entry points of at STARTUP
  if ($type eq "startup" && $when =~ /\s*STARTUP\s*/i) {
    ($wrapper_file, $proto, $out, $routine) = &parse_schedule_at_STARTUP($thorn,$privategroups,$privatevars,$implementation,$routine,$desc,@block);
  } elsif ($type eq "rfr" && $when !~ /\s*STARTUP\s*/i) {
    ($wrapper_file,$proto,$out, $routine) = &parse_schedule_at_RFR($thorn,$privategroups,$privatevars,$implementation,$routine,$when,$desc,@block);
  }

  return ($wrapper_file,$proto,$out, $routine);

}

sub parse_schedule_at_STARTUP {

  local($thorn,$privategroups,$privatevars,$implementation,$routine,$desc,@block) = @_;
  local($out);

  $out .= "  $routine();\n";

  return ("", "", $out, $routine);

}





sub parse_schedule_at_RFR {

  local($thorn,$privategroups,$privatevars,$implementation,$routine,$when,$desc,@block) = @_;
  local($proto,$out,$got_it,$i,$line);

# Look for the Language and register routine
  $got_it = 0;
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*LANG[^\:]*:\s*FORTRAN\s*$/i)
    {
      $got_it++;

      # Write the rfr called fortran wrapper routine
      $wrapper_file = &fortran_wrapper($thorn,$routine);
      $routine = "$routine"."_wrapper";
      $proto = "void $wrapper_file(CCTK_CARGUMENTS);\n"; 
   
    }
    elsif ($line =~ m/\s*LANG[^\:]*:\s*C\s*$/i)
    {
      $proto = "void $routine(CCTK_CARGUMENTS);\n"; 
      $got_it++;
    }
  }
  if ($got_it != 1) 
  {
    print "Error in LANG in schedule.ccl $got_it\n";
  }

  $out .= "  rfrRegisterFunction(GH->rfr_top,GH,$routine,$when,$desc);\n";
 
# Look for Storage
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*STOR[^\:]*:\s*(.*)\s*$/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {

	# Strip of any spaces 
	$group =~ s/ //g;

	# Take of implementation if it is there
	$this_imp = $implementation;
	$this_group = $group;
	if ($group =~ /(.*)::(.*)/)
	{
	  $this_imp = $1;
	  $this_group = $2;
	}

	if ($privategroups =~ /\b$this_group\b/)
	{
	  $use_imp = $thorn;
	}
	else
	{
	  $use_imp = $this_imp;
	}

	$out .= "  index = CCTK_GroupIndex(\"$use_imp\:\:$this_group\");\n";
	$out .= "  if (index < 0) {\n";
	$out .= "    CCTK_WARN(0,\"CCTK_GroupIndex(\\\"$use_imp\:\:$this_group\\\") failed in $thorn rfr file\\n\");\n";
        $out .= "  } else {\n"; 
	$out .= "    rfrRegisterStorage(GH->rfr_top,GH,$routine,index);\n";
        $out .= "  }\n";
      }
    }
  }
 
# Look for Communications
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*COMM[^\:]*:\s*(.*)\s*/i)
    {
      @list = split(",",$2);
      foreach $group (@list) 
      {

	# Strip of any spaces 
	$group =~  s/ //g;

	# Take of implementation if it is there
	$this_imp = $implementation;
	$this_group = $group;
	if ($group =~ /(.*)::(.*)/)
	{
	  $this_imp = $1;
	  $this_group = $2;
	}

	if ($privategroups =~ /\b$this_group\b/)
	{
	  $use_imp = $thorn;
	}
	else
	{
	  $use_imp = $this_imp;
	}

	$out .= "  index = CCTK_GroupIndex(\"$use_imp\:\:$this_group\");\n";
	$out .= "  if (index < 0) {\n";
	$out .= "    CCTK_WARN(0,\"CCTK_GroupIndex(\\\"$use_imp\:\:$this_group\\\") failed in $thorn rfr file\\n\");\n";
        $out .= "  } else {\n";
        $out .= "    rfrRegisterCommunication(GH->rfr_top,GH,$routine,index);\n";
        $out .= "  }\n";
      }
    }
  }

# Look for Triggers
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*TRIGGER(S)?\s*:\s*(.*)\s*/i)
    {
      @list = split(",",$2);
      foreach $var (@list) 
      {
	# Strip of any spaces 
	$var =~ s/ //g;

	# Take of implementation if it is there
	$this_imp = $implementation;
	$this_var = $var;
	if ($var =~ /(.*)::(.*)/)
	{
	  $this_imp = $1;
	  $this_var = $2;
	}

	if ($privategroups =~ /\b$this_var\b/ || $privatevars =~ /\b$this_var\b/ )
	{
	  $use_imp = $thorn;
	}
	else
	{
	  $use_imp = $this_imp;
	}

        $out .="\n\n";
        $out .="  index = CCTK_VarIndex(\"$use_imp\:\:$this_var\");\n";
	$out .="  if (index < 0)\n";
	$out .="  {\n";
	$out .="    index = CCTK_GroupIndex(\"$use_imp\:\:$this_var\");\n";
	$out .="    if (index >= 0)\n";
        $out .="    {\n";
        $out .="      varfirst = CCTK_FirstVarIndexI(index);\n";
        $out .="      varlast  = varfirst+CCTK_NumVarsInGroupI(index)-1;\n";
        $out .="    }\n";
        $out .="    else\n";
        $out .="    {\n";
	$out .="      CCTK_WARN(0,\"CCTK_VarIndex(\\\"$use_imp\:\:$this_var\\\") failed in $thorn rfr file\");\n";
        $out .="    }\n";
        $out .="  }\n";
        $out .="  else\n";
	$out .="  {\n";
        $out .="    varfirst = index;\n";
        $out .="    varlast  = index;\n";
        $out .="  }\n";
        $out .="  for (varindex = varfirst; varindex <= varlast; varindex++)\n";
        $out .="  {\n";
        $out .="    rfrRegisterTriggers(GH->rfr_top,GH,$routine,varindex);\n";
        $out .="  }\n";

      }
    }
  }

  return ($wrapper_file,$proto,$out, $routine);

}


#/*@@
#  @routine    fortran_wrapper
#  @date       27 Jan 1999
#  @author     Gab Allen
#  @desc 
#  Writes C wrapper for a fortran routine which is called
#  by the RFR. Note that this is done because of the arguement
#  lists. It is assumed that we are in the correct directory 
#  for writing ( configs/<config>/build/bindings/Scheduler/ )
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
#include \"cctk_Flesh.h\"
#include \"cctk_Groups.h\"
#include \"cctk_Comm.h\"
#include \"cctk_arguments.h\"

   void FORTRAN_NAME($routine)($THORN_C2F_PROTO);

   void $wrapper(CCTK_CARGUMENTS) {

   DECLARE_CCTK_CARGUMENTS
   DECLARE_$THORN_C2F
   INITIALISE_$THORN_C2F

#ifdef DEBUG
   printf(\"Calling \%s from rfr wrapper\\n\");
#endif

   FORTRAN_NAME($routine)(PASS_$THORN_C2F(cctkGH));

}
EOT

# Return the filename since we have to add it to a makefile
return ($wrapper);

}

1;
