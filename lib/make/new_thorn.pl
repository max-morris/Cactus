#!/usr/bin/perl -s
#/*@@
#  @file      new_thorn.pl
#  @date      Wed Feb  3 16:28:43 1999
#  @author    Tom Goodale
#  @desc
#  Script to make a new thorn
#  @enddesc
#  @version $Id$
#@@*/

my $cctk_home = `pwd`;
chomp ($cctk_home);

my $documentation_inputfile = "$cctk_home/doc/ThornGuide/template.tex";

$package_dir = "arrangements";

$thorn_name = shift(@ARGV);

while(&TestName(1,$thorn_name)==0)
{
  $thorn_name = &prompt("Thorn name");
}

if(!$package)
{
  @arrangements = &GetToolkits($package_dir);

  print "The following arrangements are available:\n";
  foreach $package (@arrangements)
  {
    print "$package\n";
  }
  print "Pick one, or create a new one.\n";
  while (&TestName(0,$package)==0)
  {
    $package = &prompt("arrangement");
  }
}

do {
   push @author_names,  &prompt("Author Name");
   push @author_emails, &prompt("Email Address");
   $another_author = &prompt("Add another author? Y/N");
} while ($another_author =~ /^y/i);



chdir $package_dir;

if(! -d "$package")
{
  print "Creating new arrangement $package\n";

  mkdir($package, 0755);

}

chdir $package;

if( -e $thorn_name)
{
  die "Thorn $thorn_name already exists !\n";
}

print "Creating thorn $thorn_name in $package\n";
mkdir($thorn_name, 0755);

chdir $thorn_name;

mkdir("src", 0755);
mkdir("doc", 0755);
mkdir("par", 0755);
mkdir("test", 0755);

open(OUT, ">interface.ccl") || die "Cannot create interface.ccl";

print OUT "# Interface definition for thorn $thorn_name\n";
print OUT "# \$" . "Header:\$\n";

close OUT;

open(OUT, ">param.ccl") || die "Cannot create param.ccl";

print OUT "# Parameter definitions for thorn $thorn_name\n";
print OUT "# \$" . "Header:\$\n";

close OUT;

open(OUT, ">schedule.ccl") || die "Cannot create schedule.ccl";

print OUT "# Schedule definitions for thorn $thorn_name\n";
print OUT "# \$" . "Header:\$\n";

close OUT;

open(OUT, ">README") || die "Cannot create README";

print OUT "Cactus Code Thorn $thorn_name\n";
print OUT "Authors    : ";
for ($i = 0; $i < (@author_names); $i++) {
   if ($i ne 0) {
      print OUT "\n             ";
   }
   print OUT "$author_names[$i] <$author_emails[$i]>";
}
print OUT "\n";
print OUT "CVS info   : \$" . "Header:\$\n";
print OUT "--------------------------------------------------------------------------\n";
print OUT "\n";
print OUT "Purpose of the thorn:\n";
print OUT "\n";

close OUT;

chdir("src");

open(OUT, ">make.code.defn") || die "Cannot create make.code.defn";

print OUT "# Main make.code.defn file for thorn $thorn_name\n";
print OUT "# \$" . "Header:\$\n";
print OUT "\n";
print OUT "# Source files in this directory\n";
print OUT "SRCS = \n";
print OUT "\n";
print OUT "# Subdirectories containing source files\n";
print OUT "SUBDIRS = \n";
print OUT "\n";

close OUT;

my $documentation_outputfile = "$cctk_home/$package_dir/$package/$thorn_name/doc/documentation.tex";
open(IN,  "<$documentation_inputfile") || die "Cannot open $documentation_inputfile";
open(OUT, ">$documentation_outputfile") || die "Cannot create $documentation_outputfile";

while (<IN>) {
   if (/^\\author\{\s*?\}/) {
      print OUT "\\author\{";
      for ($i = 0; $i < (@author_names); $i++) {
         if ($i ne 0) {
            print OUT " \\\\ ";
         }
         print OUT "$author_names[$i] \$<\$$author_emails[$i]\$>\$";
      }
      print OUT "\}\n";
   } elsif (/^\\date\{\s*?\}/) {
      my $todays_date = `date "+%B %d %Y"`;
      chomp ($todays_date);
      if ($todays_date =~ /^\w+\s+\d+\s+\d+$/) {
         print OUT "\\date\{$todays_date\}\n";
      } else {
         print OUT $_;
      }
   } else {
      print OUT $_;
   }
}
#system("cp $documentation_inputfile $documentation_outputfile");

print "All done.  \nPlease remember to fill out the README and doc/documentation.tex files.\n";

exit;

#/*@@
#  @routine    prompt
#  @date       Wed Feb  3 16:37:12 1999
#  @author     Tom Goodale
#  @desc
#  Prompts for something, with an optional default.
#  Based on defprompt in Cactus 3.0 Runtest
#  @enddesc
#@@*/

sub prompt {
    local ($prompt, $default) = @_;
    local ($result);
    local ($response);

    while(!$result)
    {
      if($default)
      {
	print "$prompt [$default] \n";
      }
      else
      {
	print "$prompt \n";
      }

      print "   --> ";

      $response = <STDIN>;

      if ($response =~ m/^\s*$/ && $default)
      {
        $result = $default;
      }
      elsif ($response !~ m/^\s*$/)
      {
	$result = $response;
      }
    }

    $result =~ s/\n//;
    print "\n";
    return $result;
}



#/*@@
#  @routine    GetToolkits
#  @date       Wed Feb  3 16:45:22 1999
#  @author     Tom Goodale
#  @desc
#  Gets a list of the current arrangements.
#  @enddesc
#@@*/

sub GetToolkits
{
  local($package_dir) = @_;
  local($start_dir);
  local(@arrangements);

  $start_dir = `pwd`;
  chomp($start_dir);

  chdir $package_dir;

  open(PACKAGES, "ls|");

  while(<PACKAGES>)
  {
    chop;

    # Ignore CVS and backup stuff
    next if (m:^CVS$:);
    next if (m:^\#:);
    next if (m:~$:);

    # Just pick directories
    if( -d $_)
    {
      push (@arrangements, $_);
    }
  }

  close PACKAGES;

  chdir $start_dir;

  return @arrangements;
}

#/*@@
#  @routine    TestName
#  @date       Sat Dec 16 1.48
#  @author     Gabrielle Allen
#  @desc
#  Check thorn/arrangement name is valid
#  @enddesc
#@@*/

sub TestName
{
  local($thorn,$name) = @_;
  local($valid);

  $valid = 1;

  if (!$name)
  {
    $valid = 0;
  }
  elsif ($name !~ /^[a-zA-Z]/)
  {
    print STDERR "Name must begin with a letter!\n\n";
    $valid = 0;
  }
  elsif ($name !~ /^[a-zA-Z0-9_]*$/)
  {
    print STDERR "Name can only contain letters, numbers or underscores!\n\n";
    $valid = 0;
  }

  if ($thorn && $name eq "doc")
  {
    print STDERR "Thorn name doc is not allowed!\n\n";
    $valid = 0;
  }

  return $valid;
}
