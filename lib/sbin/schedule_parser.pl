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

    $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};

    $thorn_rfr = "CCTK_".$thorn."_rfr";
    $thorn_startup = "CCTK_".$thorn."_startup";

    open (OUTRFR, ">$dir/Schedule/$thorn_rfr".".c") || die "Cannot open $thorn_rfr".".c";
    open (OUTSTART, ">$dir/Schedule/$thorn_startup".".c") || die "Cannot open $thorn_startup".".c";

    $header = &write_rfr_header($thorn,$thorn_rfr);
    print OUTRFR $header;
    &write_startup_header($thorn,$thorn_startup,OUTSTART);

    # Read all the data in the schedule file
    @indata = &read_file("$thorns{$thorn}/schedule.ccl");

    # Parse the data and create rfr and startup subroutines
    ($proto,$out,$wrapper_files, @retscheduledata) = &parse_schedule_ccl($thorn,$implementation,"rfr",@indata);
    print OUTRFR $proto;
    print OUTRFR $out; 
    $rfr_files .= " $thorn_rfr";
    $startup_files .= " $thorn_startup";
    push(@schedule_data, @retscheduledata);

    ($proto,$out,$schedule_wrappers) = &parse_schedule_ccl($thorn,$implementation,"startup",@indata);
    print OUTSTART $proto;
    print OUTSTART $out; 

   # The footer for the thorn RFR routine
   print OUTRFR "}\n";
   print OUTSTART "}\n";

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
  $header .= "#include \"flesh.h\"\n";
  $header .= "#include \"Comm.h\"\n";
  $header .= "#include \"Groups.h\"\n";
  $header .= "#include \"rfr_constants.h\"\n";
  $header .= "#include \"declare_parameters.h\"\n";
  $header .= "#include \"declare_arguments.h\"\n";
  $header .= "\n";
  $header .= "void $routine (cGH *GH)\n";
  $header .= "{\n";
  $header .= "  DECLARE_PARAMETERS\n";
  $header .= "  int index;\n\n";
  $header .= "\n";

  return $header;

}

sub write_startup_header {

  local($thorn,$routine,$out) = @_;

# The header for the thorn STARTUP routine

  print OUTSTART "#define THORN_IS_$thorn\n";
  print OUTSTART "#include \"cctk.h\"\n";
  print OUTSTART "#include \"flesh.h\"\n";
  print OUTSTART "#include \"rfr_constants.h\"\n";
  print OUTSTART "#include \"declare_parameters.h\"\n";
  print OUTSTART "\n";
  print OUTSTART "void $routine (void *data)\n";
  print OUTSTART "{\n";
  print OUTSTART "  DECLARE_PARAMETERS\n";
  print OUTSTART "\n";


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
  @sorted_routines = &OrderList("Scheduling error", ":ROUTINES:", %schedule_data);
  $order = 1;

  foreach $routine (@sorted_routines)
  {
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

#include "flesh.h"

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

  void Cactus_RegisterSTARTUP()
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
  local($thorn,$implementation,$type,@data) = @_;
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

      ($wrapper_file,$proto_block,$out_block, $routine) = &parse_schedule_block($thorn,$implementation,$type,@data);

      $proto .= "$proto_block"; 
      $out .= "$out_block";
      $compile_files .= " $wrapper_file";

      if(@options)
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
    elsif ($line =~ m/\s*STORAGE\s*:\s*(.*)/i)
    {
      if ($type eq "rfr")
      {
        @list = split(",",$1);
        foreach $group (@list) 
        {
	  # Strip of any spaces 
	  $group =~ /^\s*(.*)\s*$/;
	  $group = $1;

	  $this_imp = $implementation;
	  $this_group = $group;
	  if ($group =~ /(.*)::(.*)/)
       	  {
	    $this_imp = $1;
	    $this_group = $2;
	  }
          $out .= "CCTK_EnableGroupStorage(GH,\"$this_imp\::$this_group\");\n";
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
	  # Strip of any spaces 
	  $group =~ /^\s*(.*)\s*$/;
	  $group = $1;

	  $this_imp = $implementation;
	  $this_group = $group;
	  if ($group =~ /(.*)::(.*)/)
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
    if ($line =~  m/\}/)
    {
      $line =~  m/\s*\}\s*\"(.*)\"\s*/;
      $line_number = $i;
      if($1)
      {	
	$desc = "\"$1\"";
      }
      else
      {
	print STDERR "No description listed for routine '$routine' registered at '$rfr_entry'\n";
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
  local($thorn,$implementation,$type,@data)=@_;
  local($proto,$out);
  local($wrapper_file, $proto, $out);

  $wrapper_file = "";
  $proto = "";
  $out = "";

  ($routine,$when,$desc,@block) = &find_schedule_block(@data);

  # At the moment can schedule at RFR entry points of at STARTUP
  if ($type eq "startup" && $when =~ /\s*STARTUP\s*/i) {
    ($wrapper_file, $proto, $out, $routine) = &parse_schedule_at_STARTUP($thorn,$implementation,$routine,$desc,@block);
  } elsif ($type eq "rfr" && $when !~ /\s*STARTUP\s*/i) {
    ($wrapper_file,$proto,$out, $routine) = &parse_schedule_at_RFR($thorn,$implementation,$routine,$when,$desc,@block);
  }

  return ($wrapper_file,$proto,$out, $routine);

}

sub parse_schedule_at_STARTUP {

  local($thorn,$implementation,$routine,$desc,@block) = @_;
  local($out);

  $out .= "  $routine();\n";

  return ("", "", $out, $routine);

}





sub parse_schedule_at_RFR {

  local($thorn,$implementation,$routine,$when,$desc,@block) = @_;
  local($proto,$out,$got_it,$i,$line);

# Look for the Language and register routine
  $got_it = 0;
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*LANG\s*:\s*FORTRAN\s*$/i)
    {
      $got_it++;

      # Write the rfr called fortran wrapper routine
      $wrapper_file = &fortran_wrapper($thorn,$routine);
      $routine = "$routine"."_wrapper";
      $proto = "void $wrapper_file(CCTK_CARGUMENTS);\n"; 
   
    }
    elsif ($line =~ m/\s*LANG\s*:\s*C\s*$/i)
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
    if ($line =~ m/\s*STORAGE\s*:\s*(.*)\s*$/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {

	# Strip of any spaces 
	$group =~ /^\s*(.*)\s*$/;
	$group = $1;

	# Take of implementation if it is there
	$this_imp = $implementation;
	$this_group = $group;
	if ($group =~ /(.*)::(.*)/)
	{
	  $this_imp = $1;
	  $this_group = $2;
	}
	$out .= "  index = CCTK_GetGroupNum(\"$this_imp\",\"$this_group\");\n";
	$out .= "  if (index < 0) {\n";
	$out .= "    printf(\"CCTK_GetGroupNum failed in ".$thorn."_rfr.c\\n\");\n";
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
    if ($line =~ m/\s*COMMUNICATION\s*:\s*(.*)\s*/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {

	# Strip of any spaces 
	$group =~ /^\s*(.*)\s*$/;
	$group = $1;

	$this_imp = $implementation;
	$this_group = $group;
	if ($group =~ /(.*)::(.*)/)
	{
	  $this_imp = $1;
	  $this_group = $2;
	}
	$out .= "  index = CCTK_GetGroupNum(\"$this_imp\",\"$this_group\");\n";
	$out .= "  if (index < 0) {\n";
	$out .= "    printf(\"CCTK_GetGroupNum failed in ".$thorn."_rfr.c\\n\");\n";
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
    if ($line =~ m/\s*TRIGGERS\s*:\s*(.*)\s*/i)
    {
      @list = split(",",$1);
      foreach $var (@list) 
      {
	# Strip of any spaces 
	$var =~ /^\s*(.*)\s*$/;
	$var = $1;

	$this_imp = $implementation;
	$this_var = $var;
	if ($var =~ /(.*)::(.*)/)
	{
	  $this_imp = $1;
	  $this_var = $2;
	}
        $out .= "  index = CCTK_GetVarNum(\"$this_imp\",NULL,\"$var\");\n";
        $out .= "  rfrRegisterTriggers(GH->rfr_top,GH,$routine,index);\n"
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
#include \"flesh.h\"
#include \"Groups.h\"
#include \"declare_arguments.h\"

   void FORTRAN_NAME($routine)($THORN_C2F_PROTO);

   void $wrapper(CCTK_CARGUMENTS) {

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
