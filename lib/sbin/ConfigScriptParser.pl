#! /usr/bin/perl -s
#/*@@
#  @file      ConfigScriptParser
#  @date      Thu Mar 25 14:25:13 2004
#  @author    Yaakoub El-Khamra
#  @desc
#             Parses the Config Script Output
#  @enddesc
#  @version   $Header$
#@@*/

#/*@@
#  @routine   ParseConfigScript
#  @date      Thu Mar 25 14:25:13 2004
#  @author    Yaakoub El-Khamra
#  @desc
#  Parses the Config Script Ouput
#  @enddesc
#@@*/
sub ParseConfigScript
{
  my($config_dir, $provides, $lang, $script, $thorn, $cfg, $thorns, $filename)=@_;
  my($line_number, $temp);
 
  my $start_dir = `pwd`;
  chdir $config_dir;  

  # Run the configuration script in the config_dir folder
  if ($lang ne '' && $script ne '')
  {
    @data=`$lang $thorns{$thorn}/$script`;
  }
  
  my $exit_value  = $? >> 8;
  my $signal_num  = $? & 127;
  my $dumped_core = $? & 128;

  &CST_error (0,  "Configuration script for thorn $thorn returned $exit_value\n") if $exit_value;
  &CST_error (0,  "Configuration script for thorn $thorn received signal $signal_num\n") if $signal_num;
  &CST_error (0,  "Configuration script for thorn $thorn dumped core\n" ) if $dumped_core;

  for($line_number = 0; $line_number < @data; $line_number++)
  {
    my $line = $data[$line_number];

    # Parse the line
    if($line =~ m/^\s*BEGIN\s+DEFINE\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      while($line !~ m/^\s*END\s+DEFINE\s*/i)
      {
        $cfg->{"\U$thorn $provides\E DEFINE"} .= $line;
        $line_number++;
        $line = $data[$line_number];
      }
    }
    elsif($line =~ m/^\s*BEGIN\s+DEFINITION\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      while($line !~ m/^\s*END\s+DEFINITION\s*/i)
      {
        $cfg->{"\U$thorn $provides\E DEFINITION"} .= $line;
        $line_number++;
        $line = $data[$line_number];
      }    
    }
    elsif($line =~ m/^\s*BEGIN\s+DEPENDENCY\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      while($line !~ m/^\s*END\s+DEPENDENCY\s*/i)
      {
        $cfg->{"\U$thorn $provides\E DEPENDENCY"} .= $line;
        $line_number++;
        $line = $data[$line_number];
      }     
    }  
    elsif($line =~ m/^\s*INCLUDE_DIRECTORY[^\s]*\s*(.*)$/i)
    {
      $cfg->{"\U$thorn $provides\E INCLUDE_DIRECTORY"} .=' ' . $1;
    }
    elsif($line =~ m/^\s*LIBRARY_DIRECTORY[^\s]*\s*(.*)$/i)
    {
      $cfg->{"\U$thorn $provides\E LIBRARY_DIRECTORY"} .= ' ' . $1;
    }
    elsif($line =~ m/^\s*LIBRARY[^\s]*\s*(.*)$/i)
    {
      $cfg->{"\U$thorn $provides\E LIBRARY"} .= ' ' . $1;
    }
     else
    {
      &CST_error (0, "Unrecognised line in ConfigScriptParser.ccl '$line'");
    }
  }
  return ($cfg );
}

1;
