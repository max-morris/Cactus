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
  my($bindings_dir, $rhparameter_db, $rhinterface_db) = @_;
  my($start_dir);
  my($thorn);
  my(@data);

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

  foreach $thorn (sort split(" ", $rhinterface_db->{"THORNS"}))
  {
    push(@data, "  CCTKi_RegisterThorn(\"$thorn\",\"" . 
	 $rhinterface_db->{"\U$thorn\E IMPLEMENTS"} ."\");\n");
  }

  push(@data, "\n return 0;\n}\n");

  &OutputFile(".", "ImplementationBindings.c", @data);


  $dataout = "";
  $dataout .= "\n";
  $dataout .= "SRCS = ImplementationBindings.c\n\n";
 
  &WriteFile("make.code.defn",\$dataout);


  chdir $start_dir;
}

1;
