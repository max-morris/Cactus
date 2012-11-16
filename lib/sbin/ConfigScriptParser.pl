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
    my ($config_dir, $provides, $lang, $script, $thorn, $cfg) = @_;
    
    return if $lang eq '' || $script eq '';
    
    print "\n";
    print "********************************************************************************\n";
    print "Running configuration script for thorn $thorn:\n";
    
    # Run the configuration script in the config_dir folder
    chdir $config_dir;
    open (my $lines, '-|', "$lang $script");
    
    my $line_number = 0;
    while (my $line = <$lines>) {
        chomp $line; ++$line_number;
        next if ! $line;
        
        # Parse the line
        if ($line =~ m/^\s*BEGIN\s+DEFINE\s*/i) {
            $line = <$lines>; chomp $line; ++$line_number;
            while ($line !~ m/^\s*END\s+DEFINE\s*/i) {
                $cfg->{"\U$thorn $provides\E DEFINE"} .= "$line\n";
                $line = <$lines>; chomp $line; ++$line_number;
            }
        } elsif ($line =~ m/^\s*BEGIN\s+INCLUDE\s*/i) {
            $line = <$lines>; chomp $line; ++$line_number;
            while ($line !~ m/^\s*END\s+INCLUDE\s*/i) {
                $cfg->{"\U$thorn $provides\E INCLUDE"} .= "$line\n";
                $line = <$lines>; chomp $line; ++$line_number;
            }
        } elsif ($line =~ m/^\s*BEGIN\s+ERROR\s*/i) {
            $line = <$lines>; chomp $line; ++$line_number;
            while ($line !~ m/^\s*END\s+ERROR\s*/i) {
                $cfg->{"\U$thorn $provides\E ERROR"} .= "$line\n";
                print "ERROR: $line\n";
                $line = <$lines>; chomp $line; ++$line_number;
            }
        } elsif ($line =~ m/^\s*BEGIN\s+MESSAGE\s*/i) {
            $line = <$lines>; chomp $line; ++$line_number;
            while ($line !~ m/^\s*END\s+MESSAGE\s*/i) {
                $cfg->{"\U$thorn $provides\E MESSAGE"} .= "$line\n";
                print "$line\n";
                $line = <$lines>; chomp $line; ++$line_number;
            }
        } elsif ($line =~ m/^\s*BEGIN\s+MAKE_DEFINITION\s*/i) {
            $line = <$lines>; chomp $line; ++$line_number;
            while ($line !~ m/^\s*END\s+MAKE_DEFINITION\s*/i) {
                $cfg->{"\U$thorn $provides\E MAKE_DEFINITION"} .= "$line\n";
                $line = <$lines>; chomp $line; ++$line_number;
            }
        } elsif ($line =~ m/^\s*BEGIN\s+MAKE_DEPENDENCY\s*/i) {
            $line = <$lines>; chomp $line; ++$line_number;
            while ($line !~ m/^\s*END\s+MAKE_DEPENDENCY\s*/i) {
                $cfg->{"\U$thorn $provides\E MAKE_DEPENDENCY"} .= "$line\n";
                $line = <$lines>; chomp $line; ++$line_number;
            }
        } elsif ($line =~ m/^\s*INCLUDE_DIRECTORY\s+(.*)$/i) {
            $cfg->{"\U$thorn $provides\E INCLUDE_DIRECTORY"} .= " $1";
        } elsif ($line =~ m/^\s*LIBRARY_DIRECTORY\s+(.*)$/i) {
            $cfg->{"\U$thorn $provides\E LIBRARY_DIRECTORY"} .= " $1";
        } elsif ($line =~ m/^\s*LIBRARY\s+(.*)$/i) {
            $cfg->{"\U$thorn $provides\E LIBRARY"} .= " $1";
        } else {
            &CST_error(0, "Unrecognised line $line_number '$line' produced by configuration script '$script'");
        }
    }
    
    close $lines;
    my $exit_value  = $? >> 8;
    my $signal_num  = $? & 127;
    my $dumped_core = $? & 128;
    
    my $error_msg = $cfg->{"\U$thorn $provides\E ERROR"};
    chomp $error_msg;
    if ($error_msg) {
        $error_msg = "     Error message: '$error_msg'";
    } else {
        $error_msg = '     (no error message)';
    }
    
    my $msg = "Configuration script for thorn $thorn";
    &CST_error(0, "$msg returned exit code $exit_value\n$error_msg")
        if $exit_value;
    &CST_error(0, "$msg received signal $signal_num\n$error_msg")
      if $signal_num;
    &CST_error(0, "$msg dumped core\n$error_msg")
        if $dumped_core;
    
    print "Finished running configuration script for thorn $thorn.\n";
    
    # TODO: Should we abort if there was an error message, but the
    # return code indicates that there was no error?
}

1;
