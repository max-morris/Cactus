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

  local(%thorns) = @_;
  local($thorn);
  local(@rfr_file);
  local(@indata);

  # Header for main rfr file
  $header = 
   "\n\n
#include \"cactus.h\"\n\n
void thorns_rfr(pGH *GH) {\n";

  # Loop though each thorn's schedule file
  foreach $thorn (keys %thorns)
  {
   # Read the data
   @indata = &read_file("$thorns{$thorn}/schedule.ccl");

   # Parse the data and create rfr subroutine
   push(@rfr_file,"\n\n/* From $thorn (schedule.ccl) */\n\n");
   push(@rfr_file,&parse_schedule_ccl($thorn, @indata));
  }

  # Footer for main rfr file
  $footer = 
"\n
}\n";

  return (($header,@rfr_file,$footer));
}

#/*@@
#  @routine    parse_schedule_ccl
#  @date       Fri Jan 8 1999
#  @author     Gabrielle Allen
#  @desc 
#  Parse a thorns schedule.ccl file and write the 
#  corresponding rfr code
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
# 
#  @endhistory 
#@@*/

sub parse_schedule_ccl
{
  local($thorn,@data) = @_;

  for ($line_number=0; $line_number<@data; $line_number++)
  {
    $line = @data[$line_number];

    if ($line =~ m/\s*schedule\s*.*\s*at\s*.*/i)
    {
     push(@rfr_file,&parse_schedule_block(@data));
    }
    elsif ($line =~ m/\s*STORAGE\s*:\s*(.*)/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {
        push(@rfr_file,"EnableGroupStorage($group)\n");
      }
    }
    elsif ($line =~ m/\s*COMMUNICATION\s*:\s*(.*)/i)
    {
      @list = split(",",$1);
      foreach $group (@list) 
      {
        push(@rfr_file,"EnableGroupCommunication($group)\n");
      }
    }
    else
    {
     # Any other line is assumed for now to be C
     push(@rfr_file,"$line\n");
    }

  }  

  return @rfr_file;
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
  $line =~ m/\s*schedule\s*(.*)\s*at\s*(.*)/i;
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
  local(@data)=@_;
  local(@schedule_code);

  ($routine,$rfr_entry,$desc,@block) = &find_schedule_block(@data);

# Look for the Language and register routine
  $got_it = 0;
  for ($i=0; $i<@block; $i++) 
  {
    $line = @block[$i];
    if ($line =~ m/\s*LANG\s*:\s*FORTRAN\s*$/i)
    {
      push(@schedule_code,"  rfrRegisterEither(GH,$routine,NULL,$rfr_entry,$desc);\n");
      $got_it++;
    }
    elsif ($line =~ m/\s*LANG\s*:\s*C\s*$/i)
    {
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
       push(@schedule_code,"  RFR_RegisterStorage($group,$routine);\n");
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
        push(@schedule_code,"  RFR_RegisterComm($group,$routine);\n");
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
        push(@schedule_code,"  rfrRegisterTimer($group,$routine);\n");
      }
    }
  }

  return (@schedule_code);

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
