#! /usr/bin/perl -s
#/*@@
#  @file      setup
#  @date      Fri Jan  8 13:48:48 1999
#  @author    Tom Goodale
#  @desc 
#  Setup file for a new configuration in the CCTK
#  Invocation is 
#  setup_configuration [-reconfig] [-config_file=<options>] config_name
#  @enddesc 
#  @version $Header$
#@@*/

$top = `pwd`;

chop $top;

$configure = "sh $top/lib/make/configure";

$uname = `uname`;

$uname =~ s/(sn\d\d\d\d|jsimpson)/UNICOS\/mk/;

chop $uname;

if($#ARGV > -1)
{
  $config = shift(@ARGV);
}
else
{
  $config = $uname;
}

# Work out where the config directory is

if($ENV{"CONFIGS_DIR"})
{
  $configs_dir = $ENV{"CONFIGS_DIR"};
}
else
{
  $configs_dir = "configs";
}

# Replace slashes with underscores.
$config =~ s:[/\\]:_:g;

# The configs directory doesn't exist.
if (! -d "$configs_dir" && ! -l "$configs_dir")
{
  print "Completely new cactus build.  Creating config database\n";

  mkdir("$configs_dir", 0755)
  
}

chdir "$configs_dir" || die "Internal error - could't enter $configs_dir";

# The specified configuration doesn't exist
if (! -d "$config" && ! -l "$config")
{
  print "Creating new configuration $config.\n";

  mkdir("$config",0755) || die "Internal error - could't create $configs_dir/$config";

  chdir "$config" || die "Internal error - could't enter $configs_dir/$config";

  mkdir("build",0755);
  mkdir("lib",0755);
  mkdir("config-data",0755);

  chdir "config-data" || die "Internal error - could't enter $configs_dir/$config/config-data";

  &SetConfigureEnv();

  system("$configure");
  chdir "..";
  chdir "..";
}

# Rerun the configure script
if($reconfig)
{
  print "Reconfiguring $config.\n";

  chdir "$config" || die "Internal error - could't enter $configs_dir/$config";

  chdir "config-data" || die "Internal error - couldn't enter $configs_dir/$config/config-data";

  &SetConfigureEnv();

  system("$configure");
  chdir "..";
  chdir "..";
}

#/*@@
#  @routine    
#  @date       Fri Feb 19 19:53:48 1999
#  @author     Tom Goodale
#  @desc 
#  Sets the environment for running the configure script.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#
#@@*/
sub SetConfigureEnv
{
  local($line_number) = 0;
  # Set a default name for the configuration
  $ENV{"EXE"} = "cactus_$config";

  if($config_file)
  {
    # The user has specified a configuration file

    print "Using configuration options from $config_file...\n";
    open(INFILE, "<$top/$config_file") || die "Cannot open configuration file $config_file";
    
    while(<INFILE>)
    {
      $line_number++;

      #Ignore comments.
      s/\#(.*)$//g;
      
      s/\n//g;		# Different from chop...

      #Ignore blank lines
      next if (m:^\s*$:);

      # Match lines of the form 
      #     keyword value
      # or  keyword = value
      m/\s*([^\s=]*)([\s]*=?\s*)(.*)\s*/;

      if($1 && $2)
      {
	print "Setting $1 to '$3'\n";
	$ENV{"$1"} = $3;
      }
      else
      {
	print "Could not parse configuration line $line_number...\n'$_'\n";
      }

    }
    print "End of options from $config_file.\n";
    
    close(INFILE);
  }

}
