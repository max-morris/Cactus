#! /usr/bin/perl -s
#/*@@
#  @file      output_config.pl
#  @date      Tue Jan 19 18:59:59 1999
#  @author    Tom Goodale
#  @desc 
#  Outputs all configuration data
#  @enddesc 
#@@*/

sub OutputFile
{
  local($directory, $file, @data) = @_;

#  open(OUT, ">$directory/$file") || die "Can't open $file in $directory\n";
  $dataout = "";

  foreach $line (@data)
  {
    $dataout .= "$line\n";
  }

  &WriteFile("$directory/$file",$dataout);
#  close OUT;

}

1;

