#!/usr/bin/perl -s

#use strict;
use vars qw($cctk_home $debug $verbose $h $help $spacing $width $cellsintable $document_type $outdir $arrangements_dir $thornlist); 

#########################
# ->> InterLatex.pl <<- #
#########################################################################
#                                                                       #
#                      standard help function                           #
#                                                                       #
#########################################################################
if ($h || $help) {
   print "--> InterLatex.pl <--\n";
   print "Options:\n";
#   print "\t-th=        : (semi opt) thorn to process\n";
#   print "\t-arr=       : (semi opt) arrangement to process\n";
#   print "\t-processall : (opt) process all arrangements\n";
   print "\t-thornlist= : (opt) thornlist to process\n";
   print "\n";
#   print "\t-grouping=  : (opt) file ouput grouping scope (bythorn/byarrangement/all)\n";
   print "\t-directory= : (opt) dir. of arrangements (default arrangements/)\n";
   print "\t-outdir=    : (opt) directory to dump output files, default is .\n";
   print "\n";
#   print "\t-dump       : (opt) dumps output to screen, not in Latex\n";
   print "\t-document_type   : (opt) creates a TeX document, not just a table\n";
#   print "\t-section    : (opt) makes this a section of a greater document\n";
   print "\t-cellsintable= : (opt) the number of cells in any given table\n";
   print "\t-spacing=   : (opt) vertical spacing between elements (default 6mm)\n";
   print "\t-width=     : (opt) fixed width of table (default 150mm)\n";
   print "\n";
   print "\t-verbose    : (opt) gives verbose output to screen\n";
   print "\t-h/-help    : (opt) this screen\n";
   print "Example:\n";
   print "\tperl -s /lib/sbin/InterLatex.pl -processall -document -outdir=testdirectory -document \n";
   exit 0;
}

#/*@@
#  @file      InterLatex.pl
#  @date      Sun Mar  3 19:05:41 CET 2002
#  @author    Ian Kelley
#  @desc 
#  This program will take as input a thornlist, and outputs a latex table    
#  that contains the information in the thorns' interface.ccl file(s).  This latex
#  table can then be used as a stand-alone document, or as a section of a large
#  "ThornGuide"         
#  @enddesc 
#  @version 
#@@*/

# setup the cctk_home, if it doesn't exist, we leave it blank
$cctk_home  .= '/' if (($cctk_home !~ /\/$/) && (defined $cctk_home));

# set up the sbin dir, tacking cctk_home on the front
my $sbin_dir = "${cctk_home}lib/sbin";

##############
# REQUIRE(S) #
##############

# common procedures used to create the thornguide(s)
require "$sbin_dir/ThornUtils.pm";

# for use of create_parametere_database
require "$sbin_dir/interface_parser.pl";
require "$sbin_dir/CSTUtils.pl";

# for reading of the thornlist routine: %thorns = &ReadThornlist($thornlist)
require "$sbin_dir/MakeUtils.pl";

#####################
# INITIAL VARIABLES #
#####################

my $start_directory = `pwd`;
chomp ($start_directory);


my @valid_groups = qw(private public protected);
###########################
# COMMAND LINE VAR. CHECK #
###########################

$width ||= "150mm";
$spacing ||= "3mm";
$cellsintable ||= 6;

# set some variables in ThornUtils(.pm) namespace
$ThornUtils::cctk_home          = $cctk_home;
$ThornUtils::start_directory    = $start_directory;
$ThornUtils::verbose            = $verbose;
$ThornUtils::debug              = $debug;

# set some defaults
$document_type  ||= 'section';

##################
# INITIALIZATION #
##################
my %thorns;
my %arrangements;
my %system_database;
my %interface_database;
my %arrangements_database;
my %pathsToThorns;

my @listOfThorns;

ThornUtils::CreateSystemDatabase(\%system_database);

#@valid_groups = qw(private protected public);

$start_directory=`pwd`;
chomp($start_directory);

# get/setup the output directory and the arrangements directory
$outdir                 = ThornUtils::SetupOutputDirectory($outdir);
$arrangements_dir       = ThornUtils::GetArrangementsDir($arrangements_dir);

# determine thornlist, create one if one doesn't exist
if (defined $thornlist) {
   # provided by MakeUtils.pl, returns a hash with a list of the thorns in our thornlist
   %thorns       = &ReadThornlist($thornlist);
   @listOfThorns = keys %thorns;
} else {
   # we don't have a thornlist, go find all thorns in arrangements directory
   @listOfThorns = ThornUtils::CreateThornlist($arrangements_dir);
}

# this will return us a hash with keys as thorn names, and values as absolute paths to the 
# thorn's directory param.ccl can be located in that path.
#   We need this information to create a parameter database using create_parameter_database
#
# We are not doing ''one'' call to parameter_database as we easily could, because there is NO WAY
# (currently) to distinguish between two identical thorns in different arrangements.  So we
# would get stuff from Alphathorns/IOHDF5 in CactusBase/IOHDF5, or/and visa-versa. 

#ThornUtils::ClassifyThorns(\%arrangements, @listOfThorns);

%pathsToThorns      =  ThornUtils::GetThornPaths(\@listOfThorns, $arrangements_dir, "interface.ccl", 1);
%interface_database = &create_interface_database(scalar(keys %system_database), %system_database, %pathsToThorns);

%arrangements_database = &ReadInterfaceDatabase(\%interface_database);
#$arrangements_database{$arrangement}->{$thorn} = &ReadInterfaceDatabase(\%interface_database, "$arrangement/$thorn");


#foreach my $arrangement (keys %arrangements) {
#   foreach my $thorn (@{$arrangements{$arrangement}}) {
#      $arrangements_database{$arrangement}->{$thorn} = &ReadInterfaceDatabase(\%interface_database, "$arrangement/$thorn");
#   }
#}
#exit 0;
#exit 0;
# lets go through, one arrangement at a time
#foreach my $arrangement (keys %arrangements)
#{
#   print "\narrangement=$arrangement" if ($debug);
   # now each thorn in the given arrangement
 #  foreach my $thorn (@{$arrangements{$arrangement}})
 #  {
 #     print "\n\tthorn=$thorn" if ($debug);

      # get the path for this individual thorn
 #     %pathsToThorns = ThornUtils::GetThornPaths(["$arrangement/$thorn"], $arrangements_dir, "interface.ccl");

      # now we create a parameter database (for one thorn), using the function 
      # &create_parameter_database which is provided by 'parameter_parser.pl'
 #     %interface_database = &create_interface_database(scalar(keys %system_database), %system_database, %pathsToThorns);

      # Go split up the information we have recieved (again, for just this one thorn)
#      $arrangements_database{$arrangement}->{$thorn} = &ReadInterfaceDatabase(\%interface_database);
#   }
#}

#exit 0;
# just dump out the data-structure if we are in debug mode, don't create any files
if ($debug) {
   ThornUtils::Dump(\%arrangements_database);
   print "\n";
   exit 0;
} else {
   ThornUtils::ProcessAllArrangements(\%arrangements_database);
}

print "\nFinished.\n";
#%interface_database = &create_interface_database(scalar(keys %system_database), %system_database, %$thorn);
#&ReadLatexDatabase(%interface_database);

#########################################################################
#                 END OF MAIN SECTION OF THE PROGRAM                    # 
#########################################################################

#########################################################################
#                     BEGINNING OF SUB-ROUTINES                         #
#########################################################################

#########################################################################
# ReadLatexDataBase                                                     #
#   Calls parameter_parser.pl, which will read in the param.ccl file    #
#   from a single thorn and return all data as a %hash table, which we  #
#   will then parse to put into our own %hash tables named according to #
#   the variable names.                                                 #
#                                                                       #
#   %(variable_name)  : any number of hashes created with their names   #
#                       being the variable names, they then have $keys  #
#                       (descriptions) with $values (well, values)      #
#                          (e.g.) $name{"default"} = "Cactus";          #
#   @global       \                                                     #
#   @restricted    \  -> arrays containing the names of the relative    #
#   @private       /     hashes that are contained within their scopes  #
#   @shared       /             (masterlist contained in @valid_types)  #
#########################################################################
sub ReadInterfaceDatabase 
{
   my (%interfaceDatabase)      = %{$_[0]};

#   my $interested = $_[1];
 
  #my (%arrangementsDatabase)   = %{$_[1]};

   my %newDatabase;
  # my %thorn;

   foreach (sort keys %interfaceDatabase)
   {
      #next if (! (lc($_) cmp lc($interested)));

#      print "\n--> :<->: [$_] = [$interfaceDatabase{$_}]";
      print "\n--> [$_] = [$interfaceDatabase{$_}]" if (($debug>1) && ($verbose));

      # save he original key, as we will make it all lower case later
      # and need the original for hash keys
      my $old_key = $_;

      # just in case they have declared it, but put nothing in it.
      # if they did, don't waste our time parsing it.
      #next if (! $interfaceDatabase{$old_key} =~ /\w/);

      # drop the keys to lower-case, as they are all upper-case
      tr/A-Z/a-z/;
      /^(.*?)\/(.*?)( |$)/;
      my ($arrangement, $thorn) = ($1, $2);
      next if /^implementation/;
      next if ($interfaceDatabase{$old_key} !~ /\w/);

      if (/^([^\s]+) ([^\s]+) ([^\s]+) ([^\s]+) ([^\s]+) ([^\s]+)$/) {
print #STDERR "\n($1) ($2) ($3) ($4) ($5) ($6) [$interfaceDatabase{$old_key}]";
   #      $arrangementsDatabase{$arrangement}->{$thorn}->{$1}->{$2}->{$3}->{$4}->{$5}->{$6} = $interfaceDatabase{$old_key}; 
          $newDatabase{$arrangement}->{$thorn}->{$2}->{$3}->{$4}->{$5}->{$6} = $interfaceDatabase{$old_key}; 
      } elsif (/^([^\s]+) ([^\s]+) ([^\s]+) ([^\s]+) ([^\s]+)$/) {
print STDERR "\n($1) ($2) ($3) ($4) ($5) ($6) [$interfaceDatabase{$old_key}]";
#          $arrangementsDatabase{$arrangement}->{$thorn}->{$1}->{$2}->{$3}->{$4}->{$5} = $interfaceDatabase{$old_key}; 
          $newDatabase{$arrangement}->{$thorn}->{$2}->{$3}->{$4}->{$5} = $interfaceDatabase{$old_key};
      } elsif (/^([^\s]+) ([^\s]+) ([^\s]+) ([^\s]+)$/) {
         if ($2 eq "group") {
            $newDatabase{$arrangement}->{$thorn}->{"group details"}->{$3}->{$4} = $interfaceDatabase{$old_key};
         } else {
            $newDatabase{$arrangement}->{$thorn}->{$2}->{$3}->{$4} = $interfaceDatabase{$old_key};
         }
print STDERR "\n($1) ($2) ($3) ($4) ($5) ($6) [$interfaceDatabase{$old_key}]";
#          $arrangementsDatabase{$arrangement}->{$thorn}->{$1}->{$2}->{$3}->{$4} = $interfaceDatabase{$old_key}; 
      } elsif (/^([^\s]+) ([^\s]+) ([^\s]+)$/) {
print STDERR "\n($1) ($2) ($3) ($4) ($5) ($6) [$interfaceDatabase{$old_key}]";
          #$arrangementsDatabase{$arrangement}->{$thorn}->{$1}->{$2}->{$3} = $interfaceDatabase{$old_key}; 
         $newDatabase{$arrangement}->{$thorn}->{$2}->{$3} = $interfaceDatabase{$old_key};
      } elsif (/^([^\s]+) ([^\s]+)$/) {
print STDERR "\n($1) ($2) ($3) ($4) ($5) ($6) [$interfaceDatabase{$old_key}]";
    #      $arrangementsDatabase{$arrangement}->{$thorn}->{$1}->{$2} = $interfaceDatabase{$old_key}; 
         $newDatabase{$arrangement}->{$thorn}->{$2} = $interfaceDatabase{$old_key};
      } else {
         print "\n--> Confused: [$_] = [$interfaceDatabase{$_}]";
      }
 
   }

   return %newDatabase;
   #return %arrangementsDatabase;
   #delete $interfaceDatabase{$interested};
   #return \%thorn;
}

#/*@@
#  @routine   ProcessOneThorn
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#
#  @enddesc 
#  @version 
#@@*/
sub ProcessOneThorn 
{
   # get the thorn hash
   my (%thorn)      = %{$_[0]};
   my $arrangement  = $_[1];
   my $thorn        = $_[2];
   my $latex_output = "";

   my $ofh = ThornUtils::StartDocument("inter", $thorn, $outdir, $arrangement, "Interfaces", $document_type);
   
   # go get the latex for setting the width of the columns based on the longest variable we have 
   # (contained in the vaor $paragraph_len
#   if (defined $thorn{"variables"}) {
#      $latex_output = ThornUtils::SetWidth($TABLE_WIDTH, ThornUtils::CleanForLatex(FindMaxVarLen($thorn{"variables"})));
#   }

   # go through each group, then find all variables in the specific group type and generate the tables calling
   # the function &CreateLatexTable 
   &LatexTableElement(\%thorn);
 #  foreach my $group_type (sort keys %{$thorn{"groups"}})
 #  {
 #     foreach my $variable_name (sort @{$thorn{"groups"}{$group_type}})
 #     {
         # send in the 'variable' hash, the name of the variable, and the longest variable in this thorn
#         $latex_output .= &CreateLatexTable($thorn{"variables"}{$variable_name}, $variable_name);
##      }
#   }

   print $latex_output;
   ThornUtils::EndDocument($ofh, $document_type);
}


#########################################################################
# LatexTableElement                                                     #
#    Takes whatever table element is currently reffered to by $table    #
#    and prints it out into a LaTeX table.  Only nifty things it curr.  #
#    does is NOT print ranges for BOOLEAN and SHARED elements.          #
#########################################################################
sub LatexTableElement 
{
   # get the stuff passed in, first is a hash of the given variable, second is the name of the variable,
   # and third is the to-date longest variable in the thorn (for latex column formatting)
   my %thorn      = %{$_[0]};

   # get the different properties of the variable, and clean then up so we can output good latex
   my $variable_name = ThornUtils::CleanForLatex($_[1]);
   my @temp; 

   print "\n\\vspace\{$spacing\} \\subsection\*\{General\}";


   PrintVar("Implements", $thorn{"implements"});
   PrintVar("Inherits", $thorn{"inherits"}) if ($thorn{"inherits"} !~ /^$/);

   #$thorn{"implements"}
   #$thorn{"inherits"};

         my $printgridtitle = 1;
         foreach my $group_scope (@valid_groups) 
         {
            next if (! defined $thorn{$group_scope});

            my @groups = split/\s+/, $thorn{$group_scope}->{"groups"};

#            foreach (@groups) {
#               print STDERR "\n$_ is var in $group_scope [$thorn{$group_scope}->{\"groups\"}]";
#            }
            next if (@groups < 1);

            print "\n\\subsection\*\{Grid Variables\}" if ($printgridtitle);
            my $gs_scope = $group_scope;
            $gs_scope =~ tr/a-z/A-Z/;
            print "\n\\vspace\{5mm\}\\subsubsection\{$gs_scope GROUPS\}\n";
            print "\n\\vspace\{5mm\}\n\n\\begin\{tabular*\}\{$width\}\{|c|c\@\{\\extracolsep\{\\fill\}\}|rl|\} \\hline \n";
            print "~ \{\\bf Group Names\} ~ & ~ \{\\bf Variable Names\} ~  &\{\\bf Details\} ~ & ~\\\\ \n";
            print "\\hline \n";


            $printgridtitle = 0;
            my $counter = 0;
            foreach my $group (@groups) 
            {
               next if ($thorn{"group"}->{$group} =~ /^$/);
  
               if (( ! ($counter % $cellsintable)) && ($counter ne 0) )
               {
                  print "\\end\{tabular*\} \n\n";
                  print "\n\n\\vspace\{5mm\}";
                  print "\n\\vspace\{5mm\}\n\n\\begin\{tabular*\}\{$width\}\{|c|c\@\{\\extracolsep\{\\fill\}\}|rl|\} \\hline \n";
                  print "~ \{\\bf Group Names\} ~ & ~ \{\\bf Variable Names\} ~  &\{\\bf Details\} ~ & ~ \\\\ \n";
                  print "\\hline \n";
               }

               # print group name
               print ThornUtils::ToLower(ThornUtils::CleanForLatex($group));
               my $firstpass = 1;

               my @group_variables = split/\s+/, ThornUtils::CleanForLatex($thorn{"group"}{$group});
               $counter++;

               my $var_counter = 0;
               foreach my $group_detail (keys %{$thorn{"group details"}->{$group}}) 
               {
                  my $value = ThornUtils::CleanForLatex($thorn{"group details"}->{$group}->{$group_detail});

                  # print nothign as we are dealing with the same group
                  if (! $firstpass) {
                     print "~ &"; $firstpass=0;
                  }
                  my $new_counter = 1;
                  if (@temp = split/,/, $value) 
                  {
                     foreach my $val (@temp) 
                     {
                        if ($new_counter--) {
                           print " & $group_variables[$var_counter] & " . ExpandGroupName($group_detail) . " & $val \\\\ \n";  
                        } else {
                           print "& ~ & " .  ExpandGroupName($group_detail) ." & $val \\\\ \n";
                        }
                     }
                  } else {
                     print "$group_variables[$var_counter] & " . ExpandGroupName($group_detail) . " & $val \\\\ \n"; 
                  }
                  $var_counter++;
               } #foreach %{thorn...}
               print "\\hline \n";
            } #foreach @groups
            print "\\end\{tabular*\} \n\n";
            delete $thorn{$group_scope};
         } # foreach @valid_groups
         print "\n\n\\vspace\{5mm\}";

   #&PrintData("HEADER");
   #&PrintData("SOURCE");
   #&PrintData("FUNCTION");
} ## END :LatexTableElement:

sub PrintVar {
   my $title = shift;
   my $var = shift;
   
   my @temp;

   print "\n\n\\noindent \{\\bf ". ThornUtils::Translate(ThornUtils::CleanForLatex($title)) ."\}: ";

   if (@temp = split/\s+/, $var)
   {
      foreach (@temp) {
         print "\n\n" . ThornUtils::ToLower(ThornUtils::CleanForLatex($_));
      }
   } else {
      print "\n\n" . ThornUtils::ToLower(ThornUtils::CleanForLatex($var));
   }
   print "\n\\vspace\{2mm\}";
}

sub ExpandGroupName {
   my $name = shift;

   if ($name eq "stype") {
       return "stagger type";
    } elsif ($name eq "gtype") {
       return "group type";
    } elsif ($name eq "dim") { 
       return "dimensions";
    } elsif ($name eq "distrib") { 
       return "distribution";
    } elsif ($name eq "vtype") { 
       return "variable type";
    } elsif ($name eq "timelevels") { 
       return "timelevels";
    } else {
       return ThornUtils::ToLower(ThornUtils::CleanForLatex($name));
    }
}

sub PrintData 
{
   my $type = shift;
   my @todo = qw(add uses provides);
   my $heading;
   my @clean_values;
   my %add; my %uses; my %provides;

   my $section = &Translate($type."s");

   print "\n\n\\noindent \\subsection\*\{$section\}" if (($add{$type} !~ /^$/) || ($uses{$type} !~ /^$/) || ($provides{$type} !~ /^$/));
  
   foreach my $todo (@todo)
   {
      if ($$todo{$type} !~ /^$/) 
      {
         @clean_values = split/\s+/, &Clean($$todo{$type});

         $heading = &Translate($todo);
         $heading .= $heading =~ /s$/ ? "" : "s";

         print "\n\n\\noindent \{\\bf $heading\}:\n\n"; 
         foreach (@clean_values) 
         {
            my $check = "$type $_ TO";
            $check =~ tr/a-z/A-Z/;   

            if ((defined $$todo{$check}) && ($$todo{$check} ne $_)) {
               $check = "$_ to "  . $$todo{$check};
               print "\n\n" . $check;
            } else {
               print "\n\n$_";
            }
         }
      } #if
   }
} ## END :PrintData:
