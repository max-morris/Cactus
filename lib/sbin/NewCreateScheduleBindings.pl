#/*@@
#  @file      NewCreateScheduleBindings.pl
#  @date      Thu Sep 16 23:30:21 1999
#  @author    Tom Goodale
#  @desc 
#  New schedule stuff.  Should be renamed !!!
#  @enddesc 
#  @version $Header$
#@@*/

#/*@@
#  @routine    CreateScheduleBindings
#  @date       Fri Sep 17 14:16:23 1999
#  @author     Tom Goodale
#  @desc 
#  Creates the schedule bindings.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub NewCreateScheduleBindings
{
  local($bindings_dir, $n_param_database, $n_interface_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local(%schedule_database);
  local($start_dir);
  local($thorn);
  local($file_list);

  # Extract the parameter,interface, and schedule databases from the arguments.
  %parameter_database = @rest[0..2*$n_param_database-1];
  %interface_database = @rest[2*$n_param_database..2*($n_param_database+$n_interface_database)-1];
  %schedule_database = @rest[2*($n_param_database+$n_interface_database)..$#rest];
  
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

  if(! -d "include")
  {
    mkdir("include", 0755) || die "Unable to create include directory";
  }

  chdir "Schedule";

  $file_list = "";

  foreach $thorn (sort split(" ", $interface_database{"THORNS"}))
  {
    $buffer = &ScheduleCreateFile($thorn, scalar(%interface_database), 
				  %interface_database, %schedule_database);

    open(OUT, ">Schedule$thorn.c") || die "Unable to open Schedule$thorn.c";

    print OUT $buffer;

    close OUT;

    $file_list .= " Schedule$thorn.c";
  }

  chdir "$start_dir";
}  

sub ScheduleCreateFile
{
  local($thorn, $n_interface_database, @rest) = @_;
  local(%interface_database);
  local(%schedule_database);

  local($implementation);
  local($buffer, $prototypes);
  local($block, $block_buffer, $block_prototype);
  local($statement, $statement_buffer, $statement_prototype);
  local($indent, $language, $function);
  local(@mem_groups);
  local(@comm_groups);
  local(@trigger_groups);
  local(@before_list);
  local(@after_list);
  local(@while_list);
  local($outfile);

  # Extract the interface, and schedule databases from the arguments.
  %interface_database = @rest[0..2*$n_interface_database-1];
  %schedule_database = @rest[2*$n_interface_database..$#rest];
  
  $implementation = $interface_database{"\U$thorn\E IMPLEMENTS"};
  
  $buffer = $schedule_database{"\U$thorn\E FILE"};
  
  for($block = 0 ; $block < $schedule_database{"\U$thorn\E N_BLOCKS"}; $block++)
  {
    ($block_buffer, $block_prototype) = &ScheduleBlock($thorn, $implementation, $block,
						       $n_interface_database, 
						       %interface_database, %schedule_database);
    $buffer =~ s:\@BLOCK\@$block:$block_buffer:;
    $prototypes .= "$block_prototype";
  }

  for($statement = 0 ; $statement < $schedule_database{"\U$thorn\E N_STATEMENTS"}; $statement++)
  {
    ($statement_buffer, $statement_prototype) = &ScheduleStatement($thorn, $implementation, $statement, 
								   $n_interface_database,
								   %interface_database, %schedule_database);
    $buffer =~ s:\@STATEMENT\@$statement:$statement_buffer:;
    $prototypes .= "$statement_prototype";
  }
    
  print "---------------------------------\n";
  print "$thorn -> $implementation\n";

  print "\#include <stdarg.h>\n";
  print "\#define THORN_IS_$thorn\n";
  print "\#include \"cctk.h\"\n";
  print "\#include \"cctk_parameters.h\"\n";
  print "\#include \"cctk_schedule.h\"\n";
  print "\#include \"cctk_Comm.h\"\n";
  print "\n";
  print "/* Prototypes for functions to be registered. */";
  print "$prototypes\n";
  print "/*\@\@\n";
  print "  \@routine    CCTK_BindingsSchedule$thorn\n";
  print "  \@date       \n";
  print "  \@author     \n";
  print "  \@desc \n";
  print "  Creates the schedule bindings for thorn $thorn\n";
  print "  \@enddesc \n";
  print "  \@calls     \n";
  print "  \@calledby   \n";
  print "  \@history \n";
  print "\n";
  print "  \@endhistory\n"; 
  print "\n";
  print "\@\@*/\n";
  print "void CCTK_BindingsSchedule$thorn(cGH *GH)\n";
  print "{\n";
  print "  DECLARE_CCTK_PARAMETERS\n";
  print "$buffer\n";
  print "}\n";
  print "\n";

  print "---------------------------------\n";

  return $buffer;

}

sub ScheduleBlock
{
  local($thorn, $implementation, $block, $n_interface_database, @rest) = @_;
  local(%interface_database);
  local(%schedule_database);

  local($buffer, $prototype);
  local($indent, $language, $function);
  local(@mem_groups);
  local(@comm_groups);
  local(@trigger_groups);
  local(@before_list);
  local(@after_list);
  local(@while_list);


  # Extract the interface, and schedule databases from the arguments.
  %interface_database = @rest[0..2*$n_interface_database-1];
  %schedule_database = @rest[2*$n_interface_database..$#rest];

  # Extract group and routine information from the databases
  @mem_groups = &ScheduleSelectGroups($thorn, $implementation, 
				      $schedule_database{"\U$thorn\E BLOCK_$block STORAGE"},
				      %interface_database);

  @comm_groups = &ScheduleSelectGroups($thorn, $implementation, 
				       $schedule_database{"\U$thorn\E BLOCK_$block COMM"},
				       %interface_database);

  @trigger_groups = &ScheduleSelectGroups($thorn, $implementation, 
					  $schedule_database{"\U$thorn\E BLOCK_$block TRIGGER"},
					  %interface_database);
		

  @before_list = &ScheduleSelectRoutines($thorn, $implementation, 
					 $schedule_database{"\U$thorn\E BLOCK_$block BEFORE"},
					 %schedule_database);

  @after_list = &ScheduleSelectRoutines($thorn, $implementation, 
					$schedule_database{"\U$thorn\E BLOCK_$block AFTER"},
					%schedule_database);

  @while_list = &ScheduleSelectVars($thorn, $implementation, 
				    $schedule_database{"\U$thorn\E BLOCK_$block WHILE"},
				    %interface_database);


  # Start writing out the data 
  if($schedule_database{"\U$thorn\E BLOCK_$block TYPE"} eq "GROUP")
  {
    $prototype = "";
    $buffer = "  CCTK_ScheduleGroup(";
    $indent = "                     ";
    $language = "";
  }
  elsif($schedule_database{"\U$thorn\E BLOCK_$block TYPE"} eq "FUNCTION")
  {
    if($schedule_database{"\U$thorn\E BLOCK_$block LANGUAGE"} =~ m:^\s*C\s*$:i )
    {
      $language = "C";
      $function = $schedule_database{"\U$thorn\E BLOCK_$block NAME"};
    }
    elsif($schedule_database{"\U$thorn\E BLOCK_$block LANGUAGE"} =~ m:^\s*(F|F77|FORTRAN|F90)\s*$:i )
    {
      $language = "Fortran";
      $function = "FORTRAN_NAME(".$schedule_database{"\U$thorn\E BLOCK_$block NAME"} .")";
    }
    else
    {
      print STDERR "Unknown language " .$schedule_database{"\U$thorn\E BLOCK_$block LANGUAGE"} ."\n";
      $CST_errors++;
      return ("", "");
    }
    $prototype = "extern int $function(void); /* Note that this is a cheat, we just need a function pointer. */\n";
    $buffer = "  CCTK_ScheduleFunction($function,\n";
    $indent = "                        ";
    $buffer .= "$indent";
  }
  else
  {
    print STDERR "Internal error: Unknown schedule block type " . $schedule_database{"\U$thorn\E BLOCK_$block TYPE"} . "\n";
    return ("", "");
    $CST_errors++;
  }
  
  $buffer .= "\"" . $schedule_database{"\U$thorn\E BLOCK_$block NAME"} . "\"" . ",\n";
  $buffer .= $indent . "\"" . $thorn . "\"" . ",\n";
  $buffer .= $indent . "\"" . $implementation . "\"" . ",\n";
  $buffer .= $indent . "\"" . $schedule_database{"\U$thorn\E BLOCK_$block DESCRIPTION"} . "\"" . ",\n";
  $buffer .= $indent . "\"" . $schedule_database{"\U$thorn\E BLOCK_$block WHERE"} . "\"" . ",\n";
  if($language ne "")
  {
    $buffer .= $indent . "\"" . $language . "\"" . ",\n";
  }

  $buffer .= $indent . scalar(@mem_groups) . ",                       /* Number of STORAGE groups */\n";
  $buffer .= $indent . scalar(@comm_groups) . ",                       /* Number of COMM groups */\n";
  $buffer .= $indent . scalar(@trigger_groups) . ",                       /* Number of TRIGGERS groups */\n";
  $buffer .= $indent . scalar(@before_list) . ",                       /* Number of BEFORE routines */\n";
  $buffer .= $indent . scalar(@after_list) . ",                       /* Number of AFTER routines */\n";
  $buffer .= $indent . scalar(@while_list) . "                        /* Number of WHILE variables */";
  
  foreach $item (@mem_groups, @comm_groups, @trigger_groups, @before_list, @after_list, @while_list)
  {
    $buffer .= ",\n" . $indent . "\"" . $item . "\"" ;
  }

  $buffer .= ");\n\n";

  return ($buffer, $prototype);
}

sub ScheduleStatement
{
  local($thorn, $implementation, $statement, $n_interface_database, @rest) = @_;
  local(%interface_database);
  local(%schedule_database);

  local($buffer, $prototype);
  local(@groups);
  local($group);

  # Extract the interface and schedule databases from the arguments.
  %interface_database = @rest[0..2*$n_interface_database-1];
  %schedule_database = @rest[2*$n_interface_database..$#rest];

  # Extract the groups.
  @groups = &ScheduleSelectGroups($thorn, $implementation, 
				  $schedule_database{"\U$thorn\E STATEMENT_$statement GROUPS"},
				  %interface_database);

  if($schedule_database{"\U$thorn\E STATEMENT_$statement TYPE"} eq "STORAGE")
  {
    $function = "CCTK_EnableGroupStorage(GH,";
  }
  elsif($schedule_database{"\U$thorn\E STATEMENT_$statement TYPE"} eq "COMMUNICATION")
  {
    $function = "CCTK_EnableGroupComm(GH,";
  }
  else
  {
    print STDERR "Unknown statement type '" .$schedule_database{"\U$thorn\E STATEMENT_$statement TYPE"} ."'\n";
    $CST_errors++;
    return ("", "");
  }

  $prototype = "";

  foreach $group (@groups)
  {
    $buffer .= "  $function " . "\"" . $group . "\"" . ");\n"
  }

  return ($buffer, $prototype);
}

sub ScheduleSelectGroups
{
  local($thorn, $implementation, $group_list, %interface_database) = @_;
  local(@groups);
  local(@temp_list);
  local($group);

  @temp_list = split(/[,\s\n]+/, $group_list);

  foreach $group (@temp_list)
  {
    next if($group =~ m:^\s*$:);

    if($interface_database{"\U$thorn\E PRIVATE GROUPS"} =~ m:\b$group\b:i)
    {
      push(@groups, "$thorn\::$group");
    }
    elsif($interface_database{"\U$thorn\E PROTECTED GROUPS"} =~ m:\b$group\b:i)
    {
      push(@groups, "$implementation\::$group");
    }
    elsif($interface_database{"\U$thorn\E PUBLIC GROUPS"} =~ m:\b$group\b:i)
    {
      push(@groups, "$implementation\::$group");
    }
    else
    {
      print STDERR "Schedule error: Thorn $thorn - group $group doesn't exist.\n";
    }
  }

  return @groups;
}

sub ScheduleSelectRoutines
{
  local($thorn, $implementation, $routine_list, %schedule_database) = @_;
  local(@routines);
  local(@temp_list);
  local($routine);

  @temp_list = split(/[,\s\n]+/, $routine_list);

  foreach $routine (@temp_list)
  {
    next if($routine =~ m:^\s*$:);

    push(@routines, $routine);

  }

  return @routines;
}

sub ScheduleSelectVars
{
  local($thorn, $implementation, $var_list, %interface_database) = @_;
  local(@vars);
  local(@temp_list);
  local($var);

  @temp_list = split(/[,\s\n]+/, $var_list);

  foreach $var (@temp_list)
  {
    next if($var =~ m:^\s*$:);

    push(@vars, $var);
  }

  return @vars;
}

1;
