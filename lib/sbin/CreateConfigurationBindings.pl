#/*@@
#  @file      CreateConfigurationBindings.pl
#  @date      Thu Mar 25 14:25:13 2004
#  @author    Yaakoub Y El-Khamra
#  @desc
#             New Configuration.ccl script processing
#  @enddesc
#  @version   $Header$
#@@*/


require "$sbin_dir/CSTUtils.pl";

#/*@@
#  @routine    CreateScheduleBindings
#  @date       Thu Mar 25 14:25:13 2004
#  @author     Yaakoub Y El-Khamra
#  @desc
#              Creates the configuration bindings.
#  @enddesc
#@@*/
sub CreateConfigurationBindings
{
  my($bindings_dir, $cfg, $thorns)=@_;
  my($field, $providedcap, $thorn, $temp);

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

  if(! -d 'Configuration')
  {
    mkdir('Configuration', 0755) || die 'Unable to create Configuration directory';
  }

  if(! -d 'include')
  {
    mkdir('include', 0755) || die 'Unable to create include directory';
  }

  if(! -d "$build_dir")
  {
    mkdir("$build_dir", 0755) || die "Unable to create $build_dir";
  }

  foreach $thorn (sort keys %thorns)
  {
    if ($cfg->{"\U$thorn\E REQUIRES"} || $cfg->{"\U$thorn\E OPTIONAL"})
    {
      if(! -d "$bindings_dir/Configuration/$thorn")
      {
        mkdir("$bindings_dir/Configuration/$thorn", 0755) || die "Unable to create Thorn $thorn Configuration directory";
      }
    }
  }

  # this string goes into the cactus executable directly
  my @cap_libdirs = ();
  my @cap_libs    = ();

  # here we put all the PROVIDES to where they belong
  foreach $thorn (sort keys %thorns)
  {
    # we know that all the requirements have been satisfied
    # so all we need to do is put the provides where they belong
    # and make references to them from the requirements and optional
    # since we can have multiple provides, we make each capability
    # separate
    foreach $providedcap (split (' ', $cfg->{"\U$thorn\E PROVIDES"}))
    {
      my $thorn_providedcap = "\U$thorn $providedcap\E";

      if ($cfg->{"$thorn_providedcap DEFINE"})
      {
        &WriteFile("include/\U$providedcap\E.h",
                   \$cfg->{"$thorn_providedcap DEFINE"});
      }
      if ($cfg->{"$thorn_providedcap MAKE_DEFINITION"})
      {
        &WriteFile("Configuration/make.\U$providedcap\E.defn",
                   \$cfg->{"$thorn_providedcap MAKE_DEFINITION"});
      }
      if ($cfg->{"$thorn_providedcap MAKE_DEPENDENCY"})
      {
        &WriteFile("Configuration/make.\U$providedcap\E.deps",
                   \$cfg->{"$thorn_providedcap MAKE_DEPENDENCY"});
      }

      push (@cap_libs, $cfg->{"$thorn_providedcap LIBRARY"});
      push (@cap_libdirs, $cfg->{"$thorn_providedcap LIBRARY_DIRECTORY"});
    }
  }

  $cap_ldflags  = 'LIBDIRS += ' . join (' ', @cap_libdirs) . "\n";
  $cap_ldflags .= 'LIBS    += ' . join (' ', @cap_libs);

  &WriteFile("Configuration/make.link",\$cap_ldflags);

  # here we gather all the REQUIRES and OPTIONAL capabilities for each thorn
  foreach $thorn (sort keys %thorns)
  {
    my $thornDefnFile = '';
    my $thornDepsFile = '';
    my $requires_optional = $cfg->{"\U$thorn\E REQUIRES"} . ' ' .
                            $cfg->{"\U$thorn\E OPTIONAL"};
    foreach $requiredcap (split (' ', $requires_optional))
    {
      foreach $provider (sort keys %thorns)
      {
        foreach $providedcap (split (' ', $cfg->{"\U$provider\E PROVIDES"}))
        {
          next if ($requiredcap ne $providedcap);

          my $cap = "\U$provider $providedcap\E";

          if ($cfg->{"$cap MAKE_DEFINITION"})
          {
            $thornDefnFile .= "-include \$(BINDINGS_DIR)/Configuration/make.\U$providedcap\E.defn\n";
          }
          if ($cfg->{"$cap MAKE_DEPENDENCY"})
          {
            $thornDepsFile .= "-include \$(BINDINGS_DIR)/Configuration/make.\U$providedcap\E.deps\n";
          }
          if ($cfg->{"$cap INCLUDE_DIRECTORY"})
          {
            $thornDefnFile .= 'INC_DIRS += ' . $cfg->{"$cap INCLUDE_DIRECTORY"} . "\n";
          }
        }
      }
    }

    &WriteFile("Configuration/$thorn/make.configuration.defn",\$thornDefnFile)
      if ($thornDefnFile);
    &WriteFile("Configuration/$thorn/make.configuration.deps",\$thornDepsFile)
      if ($thornDepsFile);
  }
}

return 1;
