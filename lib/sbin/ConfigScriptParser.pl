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
  my($exit_value, $signal_num, $dumped_core);

  my $start_dir = `pwd`;
  chdir $config_dir;

  # Run the configuration script in the config_dir folder
  if ($lang ne '' && $script ne '')
  {
    @data=`$lang $thorns->{$thorn}/$script`;
  }
  $exit_value  = $? >> 8;
  $signal_num  = $? & 127;
  $dumped_core = $? & 128;

  for($line_number = 0; $line_number < @data; $line_number++)
  {
    $line = $data[$line_number];

    chomp $line;

    # Parse the line
    if($line =~ m/^\s*BEGIN\s+DEFINE\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      chomp $line;
      while($line !~ m/^\s*END\s+DEFINE\s*/i)
      {
        $cfg->{"\U$thorn $provides\E DEFINE"} .= $line;
        $line_number++;
        $line = $data[$line_number];
        chomp $line;

      }
    }
    elsif($line =~ m/^\s*BEGIN\s+ERROR\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      chomp $line;
      while($line !~ m/^\s*END\s+ERROR\s*/i)
      {
        $cfg->{"\U$thorn $provides\E ERROR"} .= $line . "\n";
        $line_number++;
        $line = $data[$line_number];
        chomp $line;
      }
    }
    elsif($line =~ m/^\s*BEGIN\s+MESSAGE\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      chomp $line;
      while($line !~ m/^\s*END\s+MESSAGE\s*/i)
      {
        $cfg->{"\U$thorn $provides\E MESSAGE"} .= "  " . $line . "\n";
        $line_number++;
        $line = $data[$line_number];
        chomp $line;
      }
    }
    elsif($line =~ m/^\s*BEGIN\s+MAKE_DEFINITION\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      chomp $line;
      while($line !~ m/^\s*END\s+MAKE_DEFINITION\s*/i)
      {
        $cfg->{"\U$thorn $provides\E MAKE_DEFINITION"} .= $line;
        $line_number++;
        $line = $data[$line_number];
        chomp $line;
      }
    }
    elsif($line =~ m/^\s*BEGIN\s+MAKE_DEPENDENCY\s*/i)
    {
      $line_number++;
      $line = $data[$line_number];
      chomp $line;
      while($line !~ m/^\s*END\s+MAKE_DEPENDENCY\s*/i)
      {
        $cfg->{"\U$thorn $provides\E MAKE_DEPENDENCY"} .= $line;
        $line_number++;
        $line = $data[$line_number];
        chomp $line;
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

  chomp ($error_msg = $cfg->{"\U$thorn $provides\E ERROR"});
  $error_msg = $error_msg ? "     Error message: '$error_msg'" : 
                            '     (no error message)';

  print $cfg->{"\U$thorn $provides\E MESSAGE"};
          
  $msg = "Configuration script for thorn $thorn ";
  &CST_error (0, $msg . "returned exit code $exit_value\n$error_msg")
    if ($exit_value);
  &CST_error (0, $msg . "received signal $signal_num\n$error_msg")
    if ($signal_num);
  &CST_error (0, $msg . "dumped core\n$error_msg")
    if ($dumped_core);

  return ($cfg);
}

1;
