#!/usr/local/bin/perl 

use strict;
use vars qw($h $help $cctk_home $thornlist $directory $outdir $verbose $debug $outfile);
#/*@@
#  @file      ThornGuide.pl
#  @date      Sun Mar  3 19:05:41 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Create cactus documentation based upon documenation provided by thorns 
#     in the current checkout being examined. (documentation.tex files)                               
#                                                                        
#     It will create a "manual" for this particular checkout, with an index  
#     and everything.  Note that the documentation provided by thorns may    
#     not use macros, etc. (except of course, the ones we provide globally)  
#
#     It attempts to include the output of InterLatex.pl, ParamLatex.pl and 
#     SchedLatex.pl, which if run before this program, will create output from
#     parsing the interface.ccl, param.ccl, and schedule.ccl files.
#  @enddesc 
#  @version 
#@@*/

#################
# HELP FUNCTION #
#################
if ($help || $h) {
   print "--> ThornGuide.pl <--\n";
   print "Options:\n";
   print "\t-directory= : (semi opt) directory to process\n";
   print "\t-thornlist= : (semi opt) thornlist to process\n";
   print "\t-outfile=   : (opt) output file (default ThornGuide.tex)\n";
   print "\t-outdir=    : (opt) output directory (default ./)\n";
   print "\t-verbose    : (opt) print verbose output\n";
   print "\t-h/-help    : (opt) this screen\n";

   print "Example:\n";
   print "\tperl -s /lib/sbin/ThornGuide.pl -thornlist=configs/test/ThornList -outfile=cactus.tex -outdir=configs/test/doc -verbose\n";
   exit 0;  
}

# setup the cctk_home, if it doesn't exist, we leave it blank
$cctk_home  .= '/' if (($cctk_home !~ /\/$/) && (defined $cctk_home));

# set up the sbin dir, tacking cctk_home on the front
my $sbin_dir = "${cctk_home}lib/sbin";

##############
# REQUIRE(S) #
##############

# common procedures used to create the thornguide(s)
require "$sbin_dir/ThornUtils.pm";

# for reading of the thornlist routine: %thorns = &ReadThornlist($thornlist)
require "$sbin_dir/MakeUtils.pl";

#####################
# INITIAL VARIABLES #
#####################

my $start_directory = `pwd`;
chomp ($start_directory);

# what file are we looking for? 
my $file = "documentation.tex";

# for putting all the bibs at the end
my $bibliography = "";

# specify output file 
$outfile ||= "ThornGuide.tex";

my $configname = "";

if ($outfile =~ /ThornGuide\-?(.*?)\.tex/) {
   $configname = ": $1";
}

# get the date for printing out on the first page of the documentation
my $TODAYS_DATE = `date +%B%d%Y`;
chomp $TODAYS_DATE;
$TODAYS_DATE =~ s/(.*?)(\d{2})(\d{4})/$1 $2, $3/;

# set some variables in ThornUtils(.pm) namespace
$ThornUtils::cctk_home          = $cctk_home;
$ThornUtils::start_directory    = $start_directory;
$ThornUtils::verbose            = $verbose;
$ThornUtils::debug              = $debug;

# get/setup the output directory and the arrangements directory
$outdir                 = ThornUtils::SetupOutputDirectory($outdir);
my $arrangements_dir    = ThornUtils::GetArrangementsDir($directory);

my %thorns;
my %arrangements;
my @listOfThorns;

# determine thornlist, create one if one doesn't exist
if (defined $thornlist) {
   # provided by MakeUtils.pl, returns a hash with a list of the thorns in our thornlist
   %thorns       = &ReadThornlist($thornlist);
   @listOfThorns = keys %thorns;
} else {
   # we don't have a thornlist, go find all thorns in arrangements directory
   @listOfThorns = ThornUtils::CreateThornlist($arrangements_dir);
}

# open the file for output #
open (OUT, ">$outdir$outfile") || die "\nCannot open $outdir$outfile for output: $!";

&Output_Top;
ThornUtils::ClassifyThorns(\%arrangements, @listOfThorns);

my $counter = 1;

# lets go through, one arrangement at a time
foreach my $arrangement (sort keys %arrangements)
{
   print "\n$arrangement" if ($debug);

   &Start_Arr($arrangement, $counter);

   print OUT &Read_Thorn_Doc($arrangements_dir, $arrangement, "donotshowwarnings");
   # now each thorn in the given arrangement
   foreach my $thorn (@{$arrangements{$arrangement}})
   {
      print "\n\t$thorn" if ($debug);
 
      my $contents = &Read_New_Thorn_Doc($arrangements_dir, $arrangement, $thorn);   
      if (! $contents) {
         $contents = &Read_Thorn_Doc($arrangements_dir, $arrangement, $thorn);   
      }
      &Add_Section($thorn, $contents); 
   }
   &End_Arr;
   $counter++;
}

&Output_Bottom;
print "\nFinished.\n";

#######################
## END OF MAIN STUFF ##
#######################

####################################################################
#                    BEGINNING OF SUB-ROUTINES                     #
####################################################################

#/*@@
#  @routine   Read_New_Thorn_Doc   
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Attempts to read in thorn documentation complying to our template.tex
#     format as specified in doc/ThornGuide/templatex.tex.
#
#     If it fails, it will return 0, and then we will try to do another
#     parsing of the documentation using &Read_Thorn_Doc()
#  @enddesc 
#  @version 
#@@*/
sub Read_New_Thorn_Doc 
{
   my $arrangements_dir = shift;
   my $arrangement      = shift;
   my $thorn            = shift;

   my $contents    = "";

   my $start = 0;
   my $stop  = 0;
   my $document_has_begun  = 0;

   my $path = "$arrangements_dir$arrangement/$thorn/doc";
   my $pathandfile = "$path/$file";
  
   my $title = "";
   my $author = "";
   my $date = "";
   my $cnts = "";

   open (DOC, "<$pathandfile") || print STDERR "\nCould not find documentation in $path";

   
   while (<DOC>)                            # loop through thorn doc.
   {
      if (/\\title\{(.*?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?.*?)\}/) { $title = $1; if ($title !~ /\w/) { close DOC; return 0;} }
      if (/\\author\{(.*?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?.*?)\}/) { $author = $1;}
      if (/\\date\{(.*?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?.*?)\}/) { $date = $1; $date =~ s/.*Date:(.*?)\$\s*?\$/$1/; }
      if (/% START CACTUS THORNGUIDE/) {
         $start = 1;

         while (($_ = <DOC>) && ($_ !~ /% END CACTUS THORNGUIDE/))
         {
            if (/(.*)\\begin\{abstract\}(.*)/) {
               $_ = "$1\\section\{Abstract\}$2";
            }

            if (/(.*)\\end\{abstract\}(.*)/) {
               $_ = "$1$2";
            }

            s/(\\includegraphics.*?\{)\s*?(.*?)\s*?\}/$1$path\/$2\}/g;

            next if (/^\s*?\\tableofcontents\s*?$/);

            if (/\\begin\{thebibliography/) {
               my $line;
               while (($line = <DOC>) && ($line !~ /\\end\{thebibliography/)) {
                  $bibliography .= $line;
               }
               next;
            }
            $contents .= $_;
         }
      }
   }

      $contents .= "\n\\include{${arrangement}_${thorn}_param}\n";
      $contents .= "\n\\include{" . ThornUtils::ToLower("${arrangement}_${thorn}_inter") . "\}\n";
      $contents .= "\n\\include{${arrangement}_${thorn}_schedule}\n";

   # if it never started reading, then we print some error message
   if (! $start) {
      close DOC; return 0;
   }
   
   $cnts .= "\n\{\\Large\n";
   $cnts .= "\n\\begin\{tabbing\}\n";
   $cnts .= "\n\{\\bf Author(s):\} \\= \\kill \\\\\n";
   $cnts .= "\n\{\\bf Title:\} \\> $title \\\\\n" if ($title =~ /\w/) && (lc($title) ne lc($thorn));

   # split the authors names up if we can
   $author =~ s/\\\\/,/g;
   $author =~ s/\s*?,\s*?,\s*?/,/g;
   my @authors = split/,/, $author;

   for (my $i = 0; $i < (@authors); $i++) {
      if ($i eq 0) { $cnts .= "\n\{\\bf Author(s):\} \\>"; 
      } else {
         $cnts .= "\n\\\> ";
      }
      $cnts .= "$authors[$i] \\\\\n"
      #$cnts .= "\n\{\\bf Author(s):\} \\> $author \\\\\n" if ($author =~ /\w/);
   }

   $cnts .= "\n\{\\bf Date:\} \\> $date \\\\\n" if ($date =~ /\w/);
   $cnts .= "\n\\end\{tabbing\}\n";
   $cnts .= "\n\}\n";
   $cnts .= "\n\\minitoc";
   close DOC;

   return "$cnts\n$contents";
}


#/*@@
#  @routine   Read_Thorn_Doc   
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Reads in the thorn documentation file and returns it as a string 
# 
#     This function starts reading when it hits he first section after 
#     the \begin{document} section, and continues until the \end{d...} 
#     statement.  it also adds the include paramater to include the    
#     output from ParamLatex.pl.                                       
#  @enddesc 
#  @version 
#@@*/
sub Read_Thorn_Doc 
{
   my $arrangements_dir = shift;
   my $arrangement      = shift;
   my $thorn            = shift;

   my $contents    = "";

   my $start = 0;
   my $stop  = 0;
   my $document_has_begun  = 0;

   my $path = "$arrangements_dir$arrangement/$thorn/doc";
   my $pathandfile = "$path/$file";
  
   my $title = "";
   my $author = "";
   my $date = "";
   my $cnts = "";

   open (DOC, "<$pathandfile") || print STDERR "\nCould not find documentation in $path";

   while (<DOC>)                            # loop through thorn doc.
   {
      if (/\\title\{(.*?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?(?:.*?\{.*?[^\{].*?\}.*?)?.*?)\}/) { $title = $1; if ($title !~ /\w/)  {$start = 0; last;}}
      if (/\\author\{(.*?)\}/) { $author = $1; }
      if (/\\date\{(.*?)\}/) { $date = $1; $date =~ s/.*Date:(.*?)\$\s*?\$/$1/; }
      if (/\\begin\{thebibliography/) {
         my $line;
         while (($line = <DOC>) && ($line !~ /\\end\{thebibliography/)) {
            $bibliography .= $line;
         }
         next;
      }

      next if (/^\s*?\\tableofcontents\s*?$/);

      if (/\\end\{document\}/) {            # stop reading
         $stop = 1;
         $contents .= "\n\\include{${arrangement}_${thorn}_param}\n";
         $contents .= "\n\\include{" . ThornUtils::ToLower("${arrangement}_${thorn}_inter") . "\}\n";
         $contents .= "\n\\include{${arrangement}_${thorn}_schedule}\n";
      }

      if ($start && ! $stop) {              # add to $contents
         s/(\\includegraphics.*?\{)\s*?(.*?)\s*?\}/$1$path\/$2\}/g;
        # s/(\\includegraphics.*?\{)\s*?(.*\.eps\s*?\})/$1$path\/$2/g;
         $contents .= $_;
      } elsif (/\\begin\{document\}/) {     # don't begin yet.... 1st flag
         $document_has_begun = 1; 
      }

      #if (($document_has_begun) && ( /\\section\{/ ) ) {
      if (($document_has_begun) && (( /\\section\{/ ) || (/\\abstract\{/)) ) {   # start reading
          if (/\\abstract\{/) {
             s/\\abstract\{/\\section\{Abstract\}\{/;
          }
          $start    = 1;
          $contents = $_;
          $document_has_begun     = 0;
      }
   }

   # if it never started reading, then we print some error message
   if (! $start) {
      if ($thorn ne "donotshowwarnings") {
      my $tmp = ThornUtils::CleanForLatex("$arrangement/$thorn");  

      if (-e $pathandfile) {
         $contents = "Could not parse latex documentation for $tmp ($file)";
      } else {
         $contents = "Could not find latex documentation for $tmp ($file)";
      }
      $contents .= "\n\n\\include{${arrangement}\_${thorn}\_param}\n";
      $contents .= "\n\\include{" . ThornUtils::ToLower("${arrangement}_${thorn}_inter") . "\}\n";
      $contents .= "\n\\include{${arrangement}\_${thorn}\_schedule}\n";
      }
   }
   
   else {
   $cnts .= "\n\{\\Large\n";
   $cnts .= "\n\\begin\{tabbing\}\n";
   $cnts .= "\n\{\\bf Author(s):\} \\= \\kill \\\\\n";
   $cnts .= "\n\{\\bf Title:\} \\> $title \\\\\n" if ($title =~ /\w/) && (lc($title) ne lc($thorn));
   $cnts .= "\n\{\\bf Author(s):\} \\> $author \\\\\n" if ($author =~ /\w/);
   $cnts .= "\n\{\\bf Date:\} \\> $date \\\\\n" if ($date =~ /\w/);
   $cnts .= "\n\\end\{tabbing\}\n";
   $cnts .= "\n\}\n";
   $cnts .= "\n\\minitoc";
   }
   close DOC;

   return "$cnts\n$contents";
}

#/*@@
#  @routine   Add_Section
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Adds a thorn section (chapter), mini table of contents and whatever we 
#     have parsed out from ($file) [documentation.tex]                       
#  @enddesc 
#  @version 
#@@*/
sub Add_Section 
{
   my $thorn    = shift;
   my $contents = shift;

$thorn = ThornUtils::CleanForLatex($thorn);
print OUT <<EOC;

\\chapter{$thorn}

$contents
EOC
}  

#/*@@
#  @routine   End_Arr
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Ends a cactuspart, which will normally be an arrangement 
#  @enddesc 
#  @version 
#@@*/
sub End_Arr
{
print OUT <<EOC;

\\end{cactuspart}

EOC
}  

#/*@@
#  @routine   Start_Arr
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Starts a cactuspart, which will normally be an arrangement 
#  @enddesc 
#  @version 
#@@*/
sub Start_Arr 
{
   my $arr     = shift;
   my $partnum = shift;

   $arr = ThornUtils::CleanForLatex($arr);
print OUT <<EOC;

\\begin{cactuspart}{$partnum}{$arr}{}{}
\\renewcommand{\\thepage}{\\Alph{part}\\arabic{page}}
EOC
}  


#/*@@
#  @routine   Output_Bottom
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Ends the latex document 
#  @enddesc 
#  @version 
#@@*/
sub Output_Bottom 
{
   if ($bibliography =~ /\w/) {
      #$bibliography = "\\addcontentsline\{toc\}\{chapter\}\{Bibliography\}\n\\begin\{thebibliography\}\{9\}\n$bibliography\n\\end\{thebibliography\}";
      &Start_Arr("References", $counter);
      $bibliography = "\\begin\{thebibliography\}\{9\}\n$bibliography\n\\end\{thebibliography\}";
      &End_Arr;
   }

print OUT <<EOC;

$bibliography

\\end{document}
EOC
}

#/*@@
#  @routine   Output_Top
#  @date      Sun Mar  3 01:54:37 CET 2002
#  @author    Ian Kelley
#  @desc 
#     Starts the latex document, using lots of stuff taken from the UserGuide 
#  @enddesc 
#  @version 
#@@*/
sub Output_Top 
{

print OUT  <<EOC;
\\newif\\ifpdf
\\ifx\\pdfoutput\\undefined
   \\pdffalse % we are not running PDFLaTeX
\\else
   \\pdfoutput=1 % we are running PDFLaTeX
   \\pdftrue
\\fi

\\documentclass{report}
\\usepackage{fancyhdr}

\\usepackage{minitoc}
\\usepackage{latexsym}
\\usepackage{ifthen}
\\usepackage{calc}
\\usepackage{graphicx}

\\setlength{\\mtcindent}{24pt}
\\renewcommand{\\mtcfont}{\\small\\rm}
\\setcounter{minitocdepth}{2}

\\makeatletter
\\\@addtoreset{chapter}{part}
\\makeatother

\\usepackage{tocloft}
\\addtolength{\\cftchapnumwidth}{0.5em}
\\addtolength{\\cftsecnumwidth}{0.5em}
\\addtolength{\\cftsubsecnumwidth}{0.5em}
\\addtolength{\\cftsubsubsecnumwidth}{0.5em}

% macros
\\def\\text#1{{\\rm #1}}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\\parskip = 2 pt
\\oddsidemargin = 0 cm
\\textwidth = 16 cm
\\topmargin = -1 cm
\\textheight = 24 cm

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\\def\\q{\\bf QUERY: }
\\def\\t{\\tt \\obeylines }

% MANPAGE like description setting for options, use as % \\begin{Lentry}
%\\item[text] text \\end{Lentry} \\usepackage{ifthen,calc}
\\newcommand{\\entrylabel}[1]{\\mbox{\\textsf{#1}}\\hfil}
\\newenvironment{entry}
  {\\begin{list}{}
    {\\renewcommand{\\makelabel}{\\entrylabel}
      \\setlength{\\labelwidth}{90pt}
      \\setlength{\\leftmargin}{\\labelwidth+\\labelsep}
    }
  }
  {\\end{list}} \\newlength{\\Mylen} \\newcommand{\\Lentrylabel}[1]{%
  \\settowidth{\\Mylen}{\\textsf{#1}}%
  \\ifthenelse{\\lengthtest{\\Mylen > \\labelwidth}}%
    {\\parbox[b]{\\labelwidth} % term > labelwidth
      {\\makebox[0pt][l]{\\textsf{#1}}\\\\}} %
    {\\textsf{#1}} %
  \\hfil\\relax} \\newenvironment{Lentry}
  {\\renewcommand{\\entrylabel}{\\Lentrylabel}
   \\begin{entry}}
  {\\end{entry}}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Takes three arguments - the name of the document, the revision, and
% the date.
% Additionally ther eis an optional first argument with the version number

\\newcommand{\\cactustitlepage}[4][4.0]
{
\\thispagestyle{empty}
\\setlength{\\parindent}{0mm}
\\setlength{\\parskip}{0mm}
\\vspace*{\\stretch{1}}
\\rule{\\linewidth}{1mm}
\\begin{flushright}
  \\Huge Cactus #1\\\\[5mm]
        #2
\\end{flushright}
\\rule{\\linewidth}{1mm}
\\vspace*{\\stretch{2}}
\\begin{center}
\\includegraphics[angle=0,width=5cm]{bincactus}
\\end{center}
\\vspace*{\\stretch{2}}
\\begin{center}
   \\Large #3 \\\\[3mm]
          #4
\\end{center}
\\newpage
}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\\newenvironment{cactuspart}[4]
{
  \\clearpage
  \\renewcommand{\\thepage}{\\Alph{part}\\arabic{page}}
  % Redefine the plain style
  \\fancypagestyle{plain}
  {
    \\fancyhf{} % Clear all header and footer fields
    \\lfoot{#3}
    \\cfoot{#4}
    %  \\rfoot{\\thepage/\\pageref{lastpage:\\thepart}}
    \\rfoot{\\thepage}
    \\renewcommand{\\headrulewidth}{0.0pt}  
    \\renewcommand{\\footrulewidth}{0.4pt}  
    \\renewcommand{\\thepage}{\\Alph{part}\\arabic{page}}
  }

  % Make sure it's arabic numbering
  \\pagenumbering{arabic}
  % Start the page counter at 1
  \\setcounter{page}{1}
  % Start a new part
  \\renewcommand{\\thepage}{\\Alph{part}\\arabic{page}}
  \\part{#2}
  \\setcounter{part}{#1}
  % Redefine the page
  % Set up fancy headings.
  \\lfoot{#3}
  \\cfoot{#4}
   % \\rfoot{\\thepage/\\pageref{lastpage:\\thepart}}
  \\rfoot{\\thepage}
  \\renewcommand{\\headrulewidth}{0.4pt}
  \\renewcommand{\\footrulewidth}{0.4pt}
}
{
  % Remember the last page of the part
  \\label{lastpage:\\thepart}
  \\clearpage
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\\def\\nhat{{\\hat n}} 
\\def\\vone{{v_{{}_{(1)}}}} 
\\def\\vtwo{{v_{{}_{(2)}}}}
\\def\\vthree{{v_{{}_{(3)}}}} 
\\def\\eref#1{(\\ref{#1})}


\\begin{document}
\\cactustitlepage{Thorn Guide$configname}{Date of Creation:}{$TODAYS_DATE}
\\dominitoc

\\setcounter{page}{1}

% Table of contents
\\pagenumbering{roman}

\\setcounter{tocdepth}{1}
\\tableofcontents

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\\renewcommand{\\thepart}{\\Alph{part}}
\\renewcommand{\\thechapter}{\\Alph{part}\\arabic{chapter}}
\\renewcommand{\\thepage}{\\Alph{part}\\arabic{page}}
\\pagestyle{fancy}

\\newlength{\\tableWidth}
\\newlength{\\maxVarWidth}
\\newlength{\\paraWidth}
\\newlength{\\descWidth}

\\newpage
%%%%%%%%%%%%%%%%%%%%%%%
EOC
}

