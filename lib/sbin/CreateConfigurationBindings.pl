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
  $linkerflagdirs = 'LDFLAGS +=';
  $linkerflaglibs = '';

  # here we put all the PROVIDES to where they belong
  foreach $thorn (sort keys %thorns)
  {
    # we know that all the requirements have been satisfied
    # so all we need to do is put the provides where they belong
    # and make references to them from the requirements and optional
    # since we can have multiple provides, we make each capability
    # separate
    if ($cfg->{"\U$thorn\E PROVIDES"})
    {
      $codedef = '';
      $codedep = '';
      $incdir = '';
      $lib = '';
      $libdir = '';
      $thornDefnFile = '';
      $thornDepsFile = '';

      foreach $providedcap (split (' ', $cfg->{"\U$thorn\E PROVIDES"}))
      {
        $providedcaplist .= $providedcap;
        if ( $cfg->{"\U$thorn $providedcap\E DEFINE"} )
        {
          &WriteFile("include/\U$providedcap\E.h",\$cfg->{"\U$thorn $providedcap\E DEFINE"});    
        }
        if ( $cfg->{"\U$thorn $providedcap\E DEFINITION"} )
        {
          $codedef = $cfg->{"\U$thorn $providedcap\E DEFINITION"}; 
          &WriteFile("$bindings_dir/Configuration/make.$\Uprovidedcap\E.defn",\$cfg->{"\U$thorn $providedcap\E DEFINITION"});
        } 
        if ( $cfg->{"\U$thorn $providedcap\E DEPENDENCY"} )
        {
          $codedep = $cfg->{"\U$thorn $providedcap\E DEPENDENCY"}; 
          &WriteFile("$bindings_dir/Configuration/make.\U$providedcap\E.deps",\$cfg->{"\U$thorn $providedcap\E DEPENDENCY"});
        } 
        if ( $cfg->{"\U$thorn $providedcap\E INCLUDE_DIRECTORY"} )
        {
          $incdir = $cfg->{"\U$thorn $providedcap\E INCLUDE_DIRECTORY"};
        } 
        if ( $cfg->{"\U$thorn $providedcap\E LIBRARY"} )
        {
          $lib = $cfg->{"\U$thorn $providedcap\E LIBRARY"};
        } 
        if ( $cfg->{"\U$thorn $providedcap\E LIBRARY_DIRECTORY"} )
        {
          $libdir = $cfg->{"\U$thorn $providedcap\E LIBRARY_DIRECTORY"};
        }
        
        # Now put the stuff from the provide in the required and options
        foreach $temp (sort keys %thorns)
        {
          foreach $requiredcap (split (' ', $cfg->{"\U$temp\E REQUIRES"}))           
          { 
            if ( $requiredcap eq $providedcap )
            { 
              if ( $codedef ne '' )
              { 
                $thornDefnFile .= "-include $bindings_dir/Configuration/make.\U$providedcap\E.defn\n";
              }
              if ( $codedep ne '' )
              {
                $thornDepsFile .= "-include $bindings_dir/Configuration/make.\U$providedcap\E.deps\n";
              }
              if ( $incdir ne '' )
              { 
                $thornDefnFile .= "INC_DIRS += $incdir\n";
              }
              if ( $libdir ne '' )
              { 
                $linkerflagdirs .= $libdir . " ";
              }
              if ( $lib ne '' )
              { 
                $linkerflaglibs .= $lib . " ";
              }
              if(! -d "Configuration/$temp")
              {
                mkdir("Configuration/$temp", 0755) || die "Unable to create Configuration/$temp directory";
              }
              &WriteFile("$bindings_dir/Configuration/$temp/make.configuration.defn",\$thornDefnFile);
              &WriteFile("$bindings_dir/Configuration/$temp/make.configuration.deps",\$thornDepsFile);
            }
          }
          foreach $requiredcap (split (' ', $cfg->{"\U$temp\E OPTIONAL"}))           
          { 
            if ( $requiredcap eq $providedcap )
            { 
              if ( $codedef ne '' )
              { 
                $thornDefnFile .= "-include $bindings_dir/Configuration/make.\U$providedcap\E.defn\n";
              }
              if ( $codedep ne '' )
              {
                $thornDepsFile .= "-include $bindings_dir/Configuration/make.\U$providedcap\E.deps\n";
              }
              if ( $incdir ne '' )
              { 
                $thornDefnFile .= "INC_DIRS += $incdir\n";
              }
              if ( $libdir ne '' )
              { 
                $linkerflagdirs .= $libdir . " ";
              }
              if ( $lib ne '' )
              { 
                $linkerflaglibs .= $lib . " ";
              }
              if(! -d "Configuration/$temp")
              {
                mkdir("Configuration/$temp", 0755) || die "Unable to create Configuration/$temp directory";
              }
              &WriteFile("$bindings_dir/Configuration/$temp/make.configuration.defn",\$thornDefnFile);
              &WriteFile("$bindings_dir/Configuration/$temp/make.configuration.deps",\$thornDepsFile);
            }
          }


        }   
      }

    }
  }  
  $linkerflagdirs .= $linkerflaglibs;
  &WriteFile("$bindings_dir/Configuration/make.link",\$linkerflagdirs);

}

return 1;
