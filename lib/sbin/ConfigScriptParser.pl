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
  my($line_number, $line, $temp);
 
  $start_dir = `pwd`;
  chdir $config_dir;  

  # Run the configuration script in the config_dir folder
  @data=`$lang $thorns{thorn}/$script`;

  for($line_number = 0; $line_number < @data; $line_number++)
  {
    $line = $data[$line_number];

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
    elsif($line =~ m/^\s*LIBRARY[^\s]*\s*(.*)$/i)
    {
      $cfg->{"\U$thorn $provides\E LIBRARY"} .= ' ' . $1;
    }
    elsif($line =~ m/^\s*LIBRARY_DIRECTORY[^\s]*\s*(.*)$/i)
    {
      $cfg->{"\U$thorn $provides\E LIBRARY_DIRECTORY"} .= ' ' . $1;
    }
     else
    {
      &CST_error (0, "Unrecognised line in ConfigScriptParser.ccl '$line'");
    }
  }
  return ($cfg );
}

1;
