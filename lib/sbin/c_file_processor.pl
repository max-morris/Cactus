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
#  a dummy assign. This avoid ugly warnings on some compilers.
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

$checkfor1 = "DECLARE_CCTK_PARAMETERS";
$addmacro1 = "USE_CCTK_PARAMETERS";
$domacro1  = 0;
$done1     = 0;

$checkfor2 = "DECLARE_CCTK_ARGUMENTS";
$addmacro2 = "USE_CCTK_CARGUMENTS";
$domacro2  = 0;
$done2     = 0;

$n_arg_braces = -3;
$sc =0;

$skip      = 0;
$skipstring= "CCTK_NO_AUTOUSE_MACRO";
  
$/ = ";\n";
$*=1;

## parse the file up to a ";\n"
while(<>)
{ 

## split in lines... and collect in routine;
  $piece = $_;  
  @lines = split("\n",$piece);
  foreach $mline (@lines) 
  {
    $routine.=$mline."\n";

    if ($mline=~/$skipstring/)
    {
      $skip = 1;
    }

## check if the DECLARE macros are found on a line
    if($mline=~m/$checkfor2/)
    {
      $domacro2     = 1;
      $n_arg_braces = 0;
      $trigger      = 1;
    }
    if($mline=~m/$checkfor1/)
    {
      $domacro1     = 1;
      $n_arg_braces = 0;
      $trigger      = 1;
    }
## start counting braces if there has been a DECLARE_
    if ($trigger>0) 
    { 
      while ($mline=~m/(})/g) {
        $n_arg_braces--;
      }
      while ($mline=~m/({)/g)  {
	$n_arg_braces++;
      }
    }
   
    if (($n_arg_braces == -1) && ($skip==0))
    {
##    Start adding first macro, deal with "return }"first, "}" after
      if ($domacro1) {
        if (!($routine=~s/([ \t\f]*)(return\s*\S*\s*}\s*)$/$1$addmacro1; $1$2/s))
        {
	  ($routine=~s/(}\s*$)/  $addmacro1; $1/s)
	}
        $domacro1=0;  
      }
##    Start adding second macro
      if ($domacro2) {
	if (!($routine=~s/([ \t\f]*)(return\s*\S*\s*}\s*)$/$1$addmacro2 $1$2/s)) 
	{
	  ($routine=~s/(}\s*$)/  $addmacro2 $1/s) 
        }
        $domacro2=0;
      }  
      $n_arg_braces = -2; 
      $sc           =  0;

## call the fortran namefix routine/reset routine
      fixfnames($routine);
      $routine      ="";
    }
  }
  
}
fixfnames($routine);
$routine      ="";


sub fixfnames {

  my $myroutine=shift(@_); 
  @flines=split /(;)/,$myroutine;

#  print $myroutine;

  foreach $fline (@flines)
  {
      while($fline =~ m:CCTK_FNAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

        @args = split(",", $arglist );

        $new = &fortran_name($args[$#args]);

        $fline =~ s:CCTK_FNAME\s*\(([^\)]*)\):$new:;
    }
    # DEPRECATED IN BETA 9
    while($fline =~ m:FORTRAN_NAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

        @args = split(",", $arglist );

        $new = &fortran_name($args[$#args]);

        $fline =~ s:FORTRAN_NAME\s*\(([^\)]*)\):$new:;
    }
    # END DEPRECATED IN BETA 9
    while($fline =~ m:CCTK_FORTRAN_COMMON_NAME\s*\(([^\)]*)\):)
    {
        $arglist = $1;
        $arglist =~ s:[\s\n\t]+::g;

        @args = split(",", $arglist );

        $new = &fortran_common_name($args[$#args]);

        $fline =~ s:CCTK_FORTRAN_COMMON_NAME\s*\(([^\)]*)\):$new:;
    }

    print $fline;
    
    
  }

}


