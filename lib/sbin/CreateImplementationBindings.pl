#/*@@
#  @file      CreateImplementationBindings.pl
#  @date      Sun Jul  4 17:09:54 1999
#  @author    Tom Goodale
#  @desc 
#  
#  @enddesc 
#@@*/

sub CreateImplementationBindings
{
  local($bindings_dir, $n_param_database, @rest) = @_;
  local(%parameter_database);
  local(%interface_database);
  local($start_dir);
  local($thorn);
  local(@data);

  %parameter_database = @rest[0..(2*$n_param_database)-1];
  %interface_database = @rest[2*$n_param_database..$#rest];

  if(! -d $bindings_dir)
  {
    mkdir("$bindings_dir", 0755) || die "Unable to create $bindings_dir";
  }
  $start_dir = `pwd`;

  chdir $bindings_dir;

  if(! -d "Implementations")
  {
    mkdir("Implementations", 0755) || die "Unable to create Implementations directory";
  }

  if(! -d "include")
  {
    mkdir("include", 0755) || die "Unable to create include directory";
  }

  chdir "Implementations";

  @data = ();

  push(@data, "#include <stdio.h>\n");
  push(@data, "/* FIXME - remove when ActiveThorns does not need this */\n");
  push(@data, "#include \"SKBinTree.h\"\n\n");
  push(@data, "#include \"cctk_ActiveThorns.h\"\n\n");

  push(@data, "int CCTKi_BindingsImplementationsInitialise(void)\n{\n");

  foreach $thorn (sort split(" ", $interface_database{"THORNS"}))
  {
    push(@data, "  CCTKi_RegisterThorn(\"$thorn\",\"" . 
	 $interface_database{"\U$thorn\E IMPLEMENTS"} ."\");\n");
  }

  push(@data, "\n return 0;\n}\n");

  &OutputFile(".", "ImplementationBindings.c", @data);


#  open (OUT, ">make.code.defn");
#  print OUT <<EOF;
$dataout = "";
$dataout .= "\n";
$dataout .= "SRCS = ImplementationBindings.c\n\n";
 
  &WriteFile("make.code.defn",$dataout);
#EOF

#  close OUT;

  chdir $_start_dir;
}

1;
