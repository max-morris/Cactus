#!/bin/perl
#/*@@
#  @file      c_file_processor.pl
#  @date      Fri Jan 22 18:09:47 1999
#  @author    Tom Goodale / Gerd Lanfermann
#  @desc 
#  Processes a c file replacing certain strings which can't be dealt
#  with by the normal c preprocessor.
#  
#  It also parses the C source and adds the USE macros, which perform 
#  a dummy assign. This avoid ugky warnings on some compilers.
#  This auto adding was tested on all C-thorns and it worked. Since this 
#  does not match the full C syntax, there can pathological cases, where
#  this script will not be able to put the USE stuff in the right place.
#  There is a switch to turn this auto-adding off: place the string
#  "CCTK_NO_AUTOUSE_MACRO" somewhere at the top (within a comment). 
#  Everything after will not be matched.
#  @enddesc 
#@@*/

$home = shift(@ARGV);

$fortran_name_file = "$home/fortran_name.pl";

if (! -e "$fortran_name_file" ) 
{
    die "Unable to get fortran name file $fortran_name_file!";
}

require "$fortran_name_file";

$/ = ";\n";
$*=1;

$checkfor1 = "DECLARE_CCTK_PARAMETERS";
$addmacro1 = "USE_CCTK_PARAMETERS";
$domacro1  = 0;
$done1     = 0;

$checkfor2 = "DECLARE_CCTK_CARGUMENTS";
$addmacro2 = "USE_CCTK_CARGUMENTS";
$domacro2  = 0;
$done2     = 0;

$skip      = 0;
$skipstring= "CCTK_NO_AUTOUSE_MACRO";
  
while(<>)
{
  @lines = split("\n",$_);
  foreach $mline (@lines) 
  {
    if ($mline=~/$skipstring/) 
    {
      $skip=1;
    }
    if ($skip==0) {

    if ($mline=~/$checkfor1/) {
      $domacro1 = 1;
    }
    if ($mline=~/$checkfor2/) {
      $domacro2 = 1;
    }
     
    while ($mline=~m/(})/g) {
      $par  = $par."c";
    }
    while ($mline=~m/({)/g)  {
      $par  = $par."o";
    }
    0 while $par =~s/oc//g;

#    print ">>$mline<< $par";

    
    if (($par eq "o")&&($mline=~m/return.*;/)&&(($domacro1==1)||($domacro2==1)))
    {
      $frag1="\nif(1)\n{ \n";
      $frag2="}";
      if ($domacro1==1) 
      {
        $frag1.="$addmacro1\n";
	$done1   =1; 
	$domacro1=0;
      }
      if ($domacro2==1)
      {
	$frag1.="$addmacro2\n";
	$done2   =1; 
	$domacro2=0;
      }
      $mline=~s/(return.*)/$frag1$1\n$frag2/;

    }
    if (($par eq "") &&($mline=~/}/)&&(($domacro1==1)||($domacro2==1))) 
    {
      $frag1="\n/* DUMMY MACROS STARTX */\n";
      $frag2="/* DUMMY MACROS ENDX */ ";
      if ($domacro1==1) 
      {
        $frag1.="$addmacro1\n";
	$done1   =1; 
	$domacro1=0;
      }
      if ($domacro2==1)
      {
	$frag1.="$addmacro2\n";
	$done2   =1; 
	$domacro2=0;
      }
      $mline=~s/(.*)(}.*)/$1$frag1$frag2$2/;
    } 
    }
    $line.=$mline."\n";
   }

    while($line =~ m:FORTRAN_NAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

        @args = split(",", $arglist );

        $new = &fortran_name($args[$#args]);

        $line =~ s:FORTRAN_NAME\s*\(([^\)]*)\):$new:;
    }
    while($line =~ m:FORTRAN_COMMON_NAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

        @args = split(",", $arglist );

        $new = &fortran_common_name($args[$#args]);

        $line =~ s:FORTRAN_COMMON_NAME\s*\(([^\)]*)\):$new:;
    }

    print $line;
    $line ="";
}

