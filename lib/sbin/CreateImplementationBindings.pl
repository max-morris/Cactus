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

  if(! $build_dir)
  {
    $build_dir = "$bindings_dir/build";
  }

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

  foreach $thorn (sort split(" ", $rhinterface_db->{"THORNS"}))
  {
    push(@data, "int CCTKi_BindingsThorn_$thorn(void);\n") 
  }

  push(@data, "int CCTKi_BindingsImplementationsInitialise(void)\n{\n");

  foreach $thorn (sort split(" ", $rhinterface_db->{"THORNS"}))
  {
    push(@data, "  CCTKi_BindingsThorn_$thorn();\n") 
  }

  push(@data, "\n return 0;\n}\n");

  &OutputFile(".", "ImplementationBindings.c", @data);


  $dataout = "";
  $dataout .= "\n";
  $dataout .= "SRCS = ImplementationBindings.c\n\n";
 
  &WriteFile("make.code.defn",\$dataout);

  if(! -d "$build_dir")
  {
    mkdir("$build_dir", 0755) || die "Unable to create $build_dir";
  }
  
  chdir "$build_dir";

  foreach $thorn (sort split(" ", $rhinterface_db->{"THORNS"}))
  {

    if(! -d "$thorn")
    {
      mkdir("$thorn", 0755) || die "Unable to create $build_dir/$thorn";
    }
  
    chdir "$thorn";

    $myimp = $rhinterface_db->{"\U$thorn\E IMPLEMENTS"};

    @data = ();

    push(@data, "#include <stdio.h>\n");
    push(@data, "#include \"cctki_ActiveThorns.h\"\n\n");

    push(@data, "int CCTKi_BindingsThorn_${thorn}(void)\n{\n");

    push(@data, "  int retval;\n");

    push(@data, "  const char *name[] = {\"$thorn\",0};");
    push(@data, "  const char *implementation[]={\"$myimp\",0};");

    push(@data, "  const char *ancestors[]=\n  {");
    foreach $ancestor (split(" ",$rhinterface_db->{"IMPLEMENTATION \U$myimp\E ANCESTORS"}))
    {
      push(@data, "    \"$ancestor\",");
    }
    push(@data, "    0,");
    push(@data, "  };\n");

    push(@data, "  const char *friends[]=\n  {");
    foreach $friend (split(" ",$rhinterface_db->{"IMPLEMENTATION \U$myimp\E FRIENDS"}))
    {
      push(@data, "    \"$friend\",");
    }
    push(@data, "    0,");
    push(@data, "  };\n");

    push(@data, "\n  struct iAttributeList attributes[] =");
    push(@data, "  {");
    push(@data, "    {\"name\",          {name}},");
    push(@data, "    {\"implementation\",{implementation}},");
    push(@data, "    {\"ancestors\",{ancestors}},");
    push(@data, "    {\"friends\",{friends}},");
    push(@data, "    {0,{0}},");
    push(@data, "  };\n");

    push(@data, "  retval = CCTKi_RegisterThorn(attributes);");

    push(@data, "\n  return retval;\n}\n");

    &OutputFile(".", "cctk_ThornBindings.c", @data);


    $dataout = "";
    $dataout .= "\n";
    $dataout .= "SRCS = cctk_ThornBindings.c\n\n";
 
    &WriteFile("make.code.defn",\$dataout);

    chdir "..";
  }
    
}

1;
