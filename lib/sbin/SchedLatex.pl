#!/usr/bin/perl -s

#/*@@
#  @file      SchedLatex.pl
#  @date      Sun Mar  3 19:05:41 CET 2002
#  @author    Ian Kelley
#  @desc 
#  This program will take as input a thornlist, and outputs a latex table    
#  that contains the information in the thorns' schedule.ccl file(s).  This latex
#  table can then be used as a stand-alone document, or as a section of a large
#  "ThornGuide"         
#  @enddesc 
#  @version 
#@@*/

#########################
# ->> SchedLatex.pl <<- #
#########################################################################
#                      Standard HELP Function                           #
#########################################################################
if ($h || $help) {
   print "--> SchedLatex.pl <--\n";
   print "Options:\n";
   print "\t-thornlist= : (opt) list specific thorns to process\n";
   print "\t-th=        : (semi opt) thorn to process\n";
   print "\t-arr=       : (semi opt) arrangement to process\n";
   print "\t-processall : (opt) process all arrangements\n";
   print "\n";
   print "\t-directory= : (opt) dir. of arrangements (default arrangements/)\n";
   print "\t-outdir=    : (opt) directory to dump output files, default is .\n";
   print "\n";
   print "\t-grouping=  : (opt) file ouput grouping scope (bythorn/byarrangement/all)\n";
   print "\t-width=     : (opt) fixed width of table (default 160mm)\n";
   print "\t-document   : (opt) creates a TeX document, not just a table\n";
   print "\t-section    : (opt) makes this a section of a greater document\n";
   print "\n";
   print "\t-debug      : (opt) prints thorn name on each schedule \n";
   print "\t-verbose    : (opt) gives verbose output to screen\n";
   print "\t-dump       : (opt) dumps output to screen, not in Latex\n";
   print "\t-h/-help    : (opt) this screen\n";
   print "\nExample:\n";
   print "\tperl -s /lib/sbin/SchedLatex.pl -processall -outdir=testdirectory -grouping=all -document\n";
   exit 0;
}

#########################################################################
#                                                                       #
#  This program will take as input a thorn, and output a latex table    #
#  that contains the information in that thorns schedule.ccl file.      #
#                                                                       #
#########################################################################

##############
# REQUIRE(S) #
##############
$cctk_home .= '/' if (($cctk_home !~ /\/$/) && defined $cctk_home);

$sbin_dir = "${cctk_home}lib/sbin";
require "$sbin_dir/ScheduleParser.pl";
require "$sbin_dir/CSTUtils.pl";

# common procedures used to create the thornguide(s)
require "$sbin_dir/ThornUtils.pm";

# for reading of the thornlist routine: %thorns = &ReadThornlist($thornlist)
require "$sbin_dir/MakeUtils.pl";


###########################
# COMMAND LINE VAR. CHECK #
###########################
$sort ||= "name";
$width ||= "160mm";

if (((! $th) && (! $arr)) && ((! defined $processall) && (! defined $thornlist))) {
   die "\nNo -th= or -arr= (or -processall or -thornlist) specified, nothing to process!\n";
}

$grouping ||="bythorn";

##################
# INITIALIZATION #
##################
$start_directory=`pwd`;
chomp($start_directory);
# set some variables in ThornUtils(.pm) namespace
$ThornUtils::cctk_home          = $cctk_home;
$ThornUtils::start_directory    = $start_directory;
$ThornUtils::verbose            = $verbose;
$ThornUtils::debug              = $debug;


# get/setup the output directory and the arrangements directory
$outdir                 = ThornUtils::SetupOutputDirectory($outdir);
$arrangements_dir       = ThornUtils::GetArrangementsDir($directory);

if (! $directory) {
   $directory = $arrangements_dir;
} elsif ($directory !~ /\/$/) {
   $directory .= '/';
}

if (defined $thornlist) {
   %thornlist = &ReadThornlist($thornlist);
   #&Read_ThornList($thornlist);
   $processall = 1;
}

#################################################
# FIND THORN(S)/ARRANGEMENT(S) AND CREATE LATEX #
#################################################

if (defined $document) {
   $document_type = "document";
} else {
   $document_type = "section";
} 

@arrangements = ThornUtils::FindDirectories($directory);

$ofh = ThornUtils::StartDocument("schedule", "MasterTable", $outdir, $arrangement, "Schedule", $document_type) if ((! $dump) && ($grouping eq "all"));
#&StartDocument("MasterTable") if ((! $dump) && ($grouping eq "all"));

foreach $arrangement (sort @arrangements) 
{
   @thorns = ThornUtils::FindDirectories($directory . $arrangement);

   $ofh = ThornUtils::StartDocument("schedule", "", $outdir, $arrangement, "Schedule", $document_type)  if ((! $dump) && ($grouping eq "byarrangement"));
   #&StartDocument($arrangement) if ((! $dump) && ($grouping eq "byarrangement"));
   foreach $thorn (@thorns) 
   {
      next if ($thorn !~ /\w/);

      if (($processall) || (($thorn eq $th) || ($arr eq $arrangement))) 
      { 
         ## do not create a file for this thorn if it is not in the THORNLIST (if one is specified)
	 next if ((defined $thornlist) && (! defined $thornlist{"$arrangement/$thorn"}));

         if (-e "$directory$arrangement/${thorn}/schedule.ccl") 
         {
            $ofh = ThornUtils::StartDocument("schedule", $thorn, $outdir, $arrangement, "Schedule", $document_type)  if ((! $dump) && ($grouping eq "bythorn"));
            #&StartDocument($thorn, $arrangement) if ((! $dump) && ($grouping eq "bythorn"));

            $$thorn{$thorn} = "${directory}${arrangement}/${thorn}";

            # we are selecting STDOUT so that any junk from the &create_schedule_database won't get in our output
            $filehandle = select(STDOUT);
            %parameter_database = &create_schedule_database(%$thorn);
            select($filehandle);

            &ReadLatexDatabase(%parameter_database);
            &LatexTableElement;
            #&EndDocument if ((! $dump) && ($grouping eq "bythorn"));
            ThornUtils::EndDocument($ofh, $document_type) if ((! $dump) && ($grouping eq "bythorn"));
            print "\n\\newpage" if ($grouping eq "all");

	    ###################################################
            # reset the variables in case they are used again #
            foreach my $b (%statements) {
               undef %$b;
            }
            undef %statements;

            foreach my $b (%blocks) {
               undef %$b;
            }
            undef %blocks;
            # END of reset #
            ################
         }       
      }
   }
   #&EndDocument if ((! $dump) && ($grouping eq "byarrangement")); 
   ThornUtils::EndDocument($ofh, $document_type) if ((! $dump) && ($grouping eq "byarrangement"));

}
#&EndDocument if ((! $dump) && ($grouping eq "all")); 
ThornUtils::EndDocument($ofh, $document_type) if ((! $dump) && ($grouping eq "all"));

print "\n" if ($verbose);
#########################################################################
#                 END OF MAIN SECTION OF THE PROGRAM                    # 
#########################################################################

#########################################################################
#                     BEGINNING OF SUB-ROUTINES                         #
#########################################################################

#########################################################################
# ReadLatexDataBase                                                     #
#   Calls schedule_parser.pl, which will read in the schedule.ccl file  #
#   from a single thorn and return all data as a %hash table, which we  #
#   will then parse to put into our own %hash tables named according to #
#   the variable names.                                                 #
#                                                                       #
#   %(variable_name)  : any number of hashes created with their names   #
#                       being the variable names, they then have $keys  #
#                       (descriptions) with $values (well, values)      #
#                          (e.g.) $name{"default"} = "Cactus";          #
#########################################################################
sub ReadLatexDatabase
{
  my (%parameter_database) = @_;
  my ($field);
  my ($name, $lname, $conditionals) = "";
  
  foreach $field (sort keys %parameter_database)
  {
     print STDERR "\n\"$field --> $parameter_database{$field}\"" if ($verbose);

     if ($field =~ /(.*?)\s(.*?)\s(.*)/) 
     {
        ($name,$block,$var) = ($1,$2,$3); 
        chomp($parameter_database{$field});

        $$block{$var} = $parameter_database{$field};

        $$block{"CONDITIONAL"} = "0";
        $$block{"THORN"}       = $name;

        if ($block =~ /^BLOCK/) {
           $blocks{$block} = 1;
        } else {
           $statements{$block} = 1;
        }
     }
   } #-- foreach

   # conditional blocks
   $conditionals = $parameter_database{"$name FILE"};
   foreach my $key (keys %blocks) 
   {
      $key =~ /BLOCK\_(\d+)/;
      my $b = "\@BLOCK\@$1";

      while ($conditionals =~ /\bif\b.*?\{(.*?)\}/imgs) 
      {
          my $if = $1;
          if ($if =~ /\Q$b\E/) {
             $$key{"CONDITIONAL"} = "true";
          }
      }
   }   

   #conditional statements
   $conditionals = $parameter_database{"$name FILE"};
   foreach my $key (keys %statements) 
   {
      $key  =~ /STATEMENT\_(\d+)/;
      my $b = "\@STATEMENT\@$1";

      while ($conditionals =~ /\bif\b.*?\{(.*?)\}/imgs) 
      {
          my $if = $1;
          if ($if =~ /\Q$b\E/) {
             $$key{"CONDITIONAL"} = "true";
          }
      }
   }   

   print "\n";
   return $name;
} ## END :ReadLatexDatabase:

#########################################################################
# LatexTableElement                                                     #
#    Takes whatever table element is currently reffered to by $table    #
#    and prints it out into a LaTeX table.  Only nifty things it curr.  #
#    does is NOT print ranges for BOOLEAN and SHARED elements.          #
#########################################################################
sub LatexTableElement 
{
   my $go = 1;
   my $i  = 0;
   my $printgridtitle = 1;
   my @conditional_statements;
   my @always_statements;
   my %aliases;

   print "\n\n (For Debug, the thorn is: " . &Clean($thorn) . ")" if ($debug);
 

   foreach my $st (sort keys %statements) 
   {
      my @groups = split/,/,$$st{"GROUPS"};
      my $type = $$st{"TYPE"};
 
      if ($type eq "STOR") 
      {
         if ($$st{"CONDITIONAL"}) {
            push @conditional_statements, @groups;
         } else {
            push @always_statements, @groups;
         }
      }
   }  
     
   # who has the most elements?
   $len = @conditional_statements > @always_statements ? @conditional_statements : @always_statements;

   print "\n\n\\noindent This section lists all the variables which are assigned storage by thorn ". &Clean($thorn) . ".  Storage can either last for the duration of the run ({\\bf Always} means that if this thorn is activated storage will be assigned, {\\bf Conditional} means that if this thorn is activated storage will be assigned for the duration of the run is some condition is met), or can be turned on for the duration of a schedule function.\n\n";

   print "\n\\subsection\*\{Storage\}";
   if (@conditional_statements > 0 || @always_statements > 0) 
   {
      print "\n\n\\hspace\{5mm\}\n\n \\begin\{tabular*\}\{$width\}\{ll\} \n";
      print @always_statements > 0 ? "\n\{\\bf Always:\}" : "~";
      print "& ";
      print @conditional_statements > 0 ? "\{\\bf Conditional:\} \\\\ \n"  : " ~ \\\\ \n";

      for ($i = 0; $i <= $len; $i++) 
      {
         print $always_statements[$i] !~ /^$/ ? &Clean($always_statements[$i]) : "~";
         print " & ";
         print $conditional_statements[$i] !~ /^$/ ? &Clean($conditional_statements[$i]) : "~";
         print "\\\\ \n";
      } 
      print "\\end\{tabular*\} \n\n";
   } else {
       print "NONE";
   }

   $printgridtitle = 1;
   $k = 0;

   foreach $group (sort by_block keys %blocks) 
   {
      next if ($group =~ /^$/);

      print "\n\\subsection\*\{Scheduled Functions\}" if ($printgridtitle); $printgridtitle=0;
      print "\n \\vspace\{5mm\}\n\n";
      print "\\noindent \{\\bf " . &Clean($$group{"WHERE"}) . "\} ";
      print $$group{"CONDITIONAL"} ? "  (conditional) \n\n" : "\n\n";
      print "\\hspace\{5mm\} ". ThornUtils::ToLower(&Clean($$group{"NAME"})) . " \n";
      print "\n\\hspace\{5mm\}\{\\it ". ThornUtils::ToLower(&Clean($$group{"DESCRIPTION"})) . " \} \n";
      print "\n\n\\hspace\{5mm\}\n\n \\begin\{tabular*\}\{$width\}\{cll\} \n";

      print "~& Language: &" . ThornUtils::ToLower(&Clean($$group{"LANG"})) . "\\\\ \n" if ($$group{"LANG"} !~ /^$/);
      print "~& After:    &" . ThornUtils::ToLower(&Clean($$group{"AFTER"})) . "\\\\ \n" if ($$group{"AFTER"} !~ /^$/);

      if (my @storage = split/,/, ThornUtils::ToLower(&Clean($$group{"STOR"}))  ) {
         print "~& Storage:  & "; #~ \\\\ \n" if ($$group{"STOR"} !~ /^$/);
         my $fp = 1;
         foreach (@storage) {
            if ($fp) {
               print "$_ \\\\ \n";
               $fp = 0;
            } else {
               print "~& ~ &" . $_. "\\\\ \n";
            }
         }
      } else {
         print "~& Storage:  &" . ThornUtils::ToLower(&Clean($$group{"STOR"})) . "\\\\ \n" if ($$group{"STOR"} !~ /^$/);
      }

      print "~& Triggers: &" . ThornUtils::ToLower(&Clean($$group{"TRIG"})) . "\\\\ \n" if ($$group{"TRIG"} !~ /^$/);
      print "~& Sync:     &" . ThornUtils::ToLower(&Clean($$group{"SYNC"})) . "\\\\ \n" if ($$group{"SYNC"} !~ /^$/);
 
      $aliases{$$group{"NAME"}} = $$group{"AS"};

      delete $$group{"CONDITIONAL"};
      delete $$group{"WHERE"};
      delete $$group{"DESCRIPTION"};
      delete $$group{"LANG"};
      delete $$group{"AFTER"};
      delete $$group{"STOR"};
      delete $$group{"TRIG"};
      delete $$group{"SYNC"};
      delete $$group{"AS"};
      delete $$group{"NAME"};
      delete $$group{"THORN"};

      $i = 0;
      foreach $group_key (sort keys %$group) 
      {
         next if ($$group{$group_key} =~ /^$/);

         $clean_name  = ThornUtils::Translate(&Clean($group_key));
         $clean_var   = &Clean($vars[$i]);
         $clean_value = ThornUtils::ToLower(&Clean($$group{$group_key}));

         $j = 1;
         if (@clean_values = split/,/,$clean_value) 
         {
            foreach $clean_value (@clean_values) 
            {
               next if ($clean_value !~ /\w/);
               if ($j) {
                  print $clean_var . "& $clean_name: & $clean_value  \\\\ \n";
                  $j = 0;
               } else {
                  print "~ & ~ & $clean_value \\\\ \n";
               }
            }             
         } else {
            print $clean_var . "& $clean_name: & $clean_value \\\\ \n"; 
         }

         $i++;
      }
      print "\\end\{tabular*\} \n\n";
   }

   # delete aliases where they key equals the value
   foreach my $key (keys %aliases) {
      if ($key eq $aliases{$key}) {
         delete $aliases{$key};
      }
   }

   if (scalar(keys %aliases) > 0) 
   {
      print "\n\\subsection\*\{Aliased Functions\}";
      print "\n\n\\hspace\{5mm\}\n\n \\begin\{tabular*\}\{$width\}\{ll\} \n";
      print "\n\{\\bf Alias Name:\} ~~~~~~~& \{\\bf Function Name:\} \\\\ \n";

      foreach my $key (sort keys %aliases) {
         print &Clean($key) ." & ". &Clean($aliases{$key}) ." \\\\ \n";# if ($key ne $aliases{$key});
      }
      print "\\end\{tabular*\} \n\n";
   }

   print "\n\n\\vspace\{5mm\}"; 
}

#################################################
# Cleans up a value, so latex is happy with it. #
#################################################
sub Clean 
{
   my $val = shift;

   $val =~ s/^\s*?\"//;
   $val =~ s/\"$//;
   $val =~ s/\_/\\\_/g;
   $val =~ s/\$/\\\$/g;

   return $val;
} ## END :Clean:

#########################################################################
# SortByType                                                            #
#    Sorts the ouput by "type" within their respective thorns, as we can#
#    have repetitive variable names within different thorns, currently  #
#    output is restricted to internal (within thorn) sorting.           #
#########################################################################
sub SortByType {
   if (lc($$a{"type"}) cmp lc($$b{"type"}) < 0) {
      return -1;
   } elsif (lc($$a{"type"}) cmp lc($$b{"type"}) > 0) {
      return 1;
   } else {
      return -1;
   }

} ## END :SortByType:

#########################################################################
# SortByName                                                            #
#    Sorts the ouput by "name" within their respective thorns, as we can#
#    have repetitive variable names within different thorns, currently  #
#    output is restricted to internal (within thorn) sorting.           #
#########################################################################
sub SortByName {
   my ($first)  = $$a{"name"};
   my ($second) = $$b{"name"};

   if (lc($first) cmp lc($second) < 0) {
      print "\n$first : $second -1";
      return -1;
   } elsif (lc($$a{"name"}) cmp lc($$b{"name"}) > 0) {
      print "\n$$a{\"name\"} : $$b{\"name\"} 0";
      return 1;
   } else {
      print "\n$$a{\"name\"} : $$b{\"name\"} -1 (2)";
      return -1;
   }

} ## END :SortByName:

#########################################################################
# Dump                                                                  #
#    Function to dump output to the screen rather than to a .tex file   #
#    table, this does NOT create latex, simple provides an easy way to  #
#    view the variables from the command line without use of Latex.     #
#########################################################################
sub Dump {
  my (@cur_group) = @_;
 
  print "\n$thorn variables:\n";

  foreach $value (@cur_group) 
  {
    if (lc($$value{"thorn"}) eq lc($thorn)) { 
      print "\t$value:\n";
      foreach $key (keys %$value) {
	  print "\t\t$key -> $$value{$key}\n";
      }
    }
  }

} ## END :Dump:

#########################################################################
# Clean                                                                 #
#    Function to perform any cleaning that may need to be done to       #
#    variables before they are put into a hash of their name.           #
#########################################################################
sub DUMP {
   select STDOUT;
   print "\n$thorn\n";
   foreach my $b (sort by_block keys %blocks) {
         print "\n\t$b";
         foreach my $key (sort keys %$b) {
            print "\n\t\t$key = $$b{$key}" if ($key ne "THORN");
         }
   }
   foreach my $b (sort by_block keys %statements) {
         print "\n\t$b";
         foreach my $key (sort keys %$b) {
            print "\n\t\t$key = $$b{$key}" if ($key ne "THORN");
         }
   }
}

##########################
# Sorting function       #
##########################
sub by_block {
   my $i,j;
   $a =~ /\_(.*)/;
   $i = $1;
   $b =~ /\_(.*)/;
   $j = $1;
   
   return 1 if ($i > $j);
   return -1 if ($i < $j);
   return 0; 
}

