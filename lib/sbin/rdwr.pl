use lib ".";
use Piraha;
use FileHandle;
use Carp;
use Data::Dumper;
use strict;

my $sch_file = $ENV{CCTK_HOME}."/src/piraha/pegs/schedule.peg";
my($S_grammar,$S_rule)=piraha::parse_peg_file($sch_file);

my $int_file = $ENV{CCTK_HOME}."/src/piraha/pegs/interface.peg";
my($I_grammar,$I_rule)=piraha::parse_peg_file($int_file);

sub interface_starter
{
  my $thornname = uc shift;
  my $hash = shift;
  my $gr = shift;
  my $ccl_file = shift;
  for my $ch (@{$gr->{children}}) {
    if($ch->is("FUNC_GROUP")) {
      for my $gch (@{$ch->{children}}) {
        if($gch->is("IMPLEMENTS")) {
          # This finds the implementation name for the thorn.
          my $name = uc $gch->has(0,"name")->substring();
          $hash->{$name} = {} if(!defined($hash->{$name}));
          do_interfaces($hash->{$name},$gr,$ccl_file);
          # Private variables are referenced by thorn name instead
          # of implementation name. The 'private' key stores the
          # variables under the thorn name to handle this.
          $hash->{private_variable}->{$thornname} = $hash->{$name};
          return;
        }
      }
    }
  }
}

sub do_interfaces
{
  my $hash = shift;
  my $gr = shift;
  my $ccl_file = shift;
  if($gr->is("GROUP_VARS")) {
    my $vtype = $gr->has(0,"vtype")->substring();
    my $level = 0;
    my $vecval = "0";
    my $dim = 0;
    my $gname;
    my $gtype;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("gname")) {
        $gname = $ch->has(0,"name")->substring();
        if($ch->has(1,"expr")) {
          # This section finds the length for vectors.
          my $expr = $ch->has(1,"expr")->has(0,"addexpr")->has(0,"mulexpr")->has(0,"powexpr");
          if($expr->has(0,"num")) {
            $vecval = $expr->has(0,"num")->substring();
          } elsif($expr->has(0,"accname")) {
            $vecval = $expr->has(0,"accname")->substring();
          } elsif($expr->has(0,"parexpr")) {
            $vecval = $expr->has(0,"parexpr")->substring();
          } else {
            my $loc = "(".$ccl_file."::".$expr->linenum().")";
            &CST_error(0, "Unexpected structure encountered in interface parsing at $loc"
                , "", __LINE__, __FILE__);
          }
        }
      } elsif($ch->is("gtype")) {
        $gtype = $ch->substring();
      } elsif($ch->is("dim") and (uc $gtype) eq "ARRAY") {
        $dim = $ch->substring();
      } elsif($ch->is("timelevels")) {
        $level = $ch->substring();
        last;
      }
    }
    if($level-1 < 0) {
      $hash->{$gname}->{level} = 0;
    } else {
      $hash->{$gname}->{level} = $level-1;
    }
    $hash->{$gname}->{vtype} = uc $vtype;
    $hash->{$gname}->{vector} = $vecval;
    $hash->{$gname}->{gtype} = uc $gtype;
    $hash->{$gname}->{array_dim} = $dim;
    my $Detect = 0;
    for my $ch (@{$gr->{children}}) {
      # Looping over variables in the group
      if($ch->is("VARS")) {
        my $i = 0;
        $Detect = 1;
        while($ch->has($i,"name")) {
          my $var = $ch->has($i,"name")->substring();
          $hash->{$gname}->{grp_vars}->{$var} = $var;
          $hash->{variable_list}->{$var} = $gname;
          $i++;
        }
        last;
      }
    }
    if($Detect == 0) {
      # If no VARS were detected, then the group name
      # is also the variable name.
      $hash->{$gname}->{grp_vars}->{$gname} = $gname;
      $hash->{variable_list}->{$gname} = $gname;
    }
  } else {
    for my $ch (@{$gr->{children}}) {
      do_interfaces($hash,$ch,$ccl_file);
    }
  }
}

sub schedule_starter
{
  my $tnm = uc shift;
  my $hash = shift;
  my $gr = shift;
  my $ccl_file = shift;
  my $lang = {};
  my $reads_writes = {};
  my $data = "";
  do_schedules($gr,$reads_writes,$lang,$ccl_file);
  create_macros($tnm,$hash,$gr,$reads_writes,$lang,\$data,$ccl_file);
  return $data;
}

sub do_schedules
{
  my $gr = shift;
  my $reads_writes = shift;
  my $lang = shift;
  my $ccl_file = shift;
  if($gr->is("schedule")) {
    next if($gr->has(0,"group")); #group scheduling has no rd/wr clauses
    my $nm;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("name")) {
        $nm = $ch->substring();
      } elsif($ch->is("lang")) {
        # Cactus allows for functions to have the same name if
        # they are different languages. Because of this, the
        # different functions must be distinguished for macro
        # generation. '_C' or '_F' are appended to the end of
        # the function name for clarity.
        my $language = uc $ch->has(0,"name")->substring();
        $nm .= "_".substr($language,0,1);
        $lang->{$nm} = $language;
        last;
      }
    }
    for my $ch (@{$gr->{children}}) {
      if($ch->is("reads") or $ch->is("writes")) {
        my $is_writes = $ch->is("writes");
        my $qname = $ch->has(0,"qname");
        my $thorn = uc $qname->has(0,"vname")->has(0,"name")->substring();
        my $var = $qname->has(0,"vname")->has(1,"name")->substring();
        $reads_writes->{$nm}->{$thorn}->{$var} += $is_writes;
        my $i = 1;
        $i++ if($qname->has($i,"region"));
        while($qname->has($i,"name")) {
          $var = $qname->has($i,"name")->substring();
          $reads_writes->{$nm}->{$thorn}->{$var} += $is_writes;
          $i++;
        }
        delete $reads_writes->{$nm}->{$nm}->{$nm};
      }
    }
    if(!defined($reads_writes->{$nm})) {
      # In the event that a function has no declarations,
      # an empty macro still needs to be generated. This
      # handles that case. Since a function can be scheduled
      # multiple times, this hash key is deleted if later
      # scheduling adds variables to the list of read/write
      # declarations.
      $reads_writes->{$nm}->{$nm}->{$nm} = "empty";
    }
  } else {
    for my $ch (@{$gr->{children}}) {
      do_schedules($ch,$reads_writes,$lang);
    }
  }
}

sub create_macros
{
  my $tnm = uc shift;
  my $hash = shift;
  my $gr = shift;
  my $reads_writes = shift;
  my $lang = shift;
  my $data = shift;
  my $ccl_file = shift;
  $$data .= "#ifndef CCTK_ARGUMENTS_CHECKED_H\n";
  $$data .= "#define CCTK_ARGUMENTS_CHECKED_H 1\n";
  for my $namekey (keys %{$reads_writes}) {
    my $temp_data = "";
    my $nm = substr($namekey,0,-2); # removing language suffix from function name
    if($reads_writes->{$namekey}->{$namekey}->{$namekey} eq "empty") {
      # This generates macros for functions with no read/write declarations.
      if ($lang->{$namekey} eq "C") {
        $$data .= "#ifdef CCODE \n";
        $$data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
        $$data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
        $$data .= "  _DECLARE_CCTK_ARGUMENTS; \\\n";
        $$data .= "  /* end $nm */\n";
        $$data .= "#endif\n";
        $$data .= "#endif\n";
      } elsif ($lang->{$namekey} eq "FORTRAN") {
        $$data .= "#ifdef FCODE \n";
        $$data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
        $$data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
        $$data .= "  _DECLARE_CCTK_FARGUMENTS; \\\n";
        $$data .= "  /* end $nm */\n";
        $$data .= "#endif\n";
        $$data .= "#ifndef CCTK_ARGUMENTS_${nm} \n";
        $$data .= "#define CCTK_ARGUMENTS_$nm _CCTK_ARGUMENTS \n";
        $$data .= "#endif\n";
        $$data .= "#endif\n";
      } else {
        my $loc = "(".$ccl_file."::".$lang->linenum().")";
        &CST_error(0, "Failed to match the language for the function $nm at $loc"
            ,"", __LINE__, __FILE__);
      }
    } else {
      if($lang->{$namekey} eq "C") {
        $$data .= "#ifdef CCODE \n";
        $$data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
        $$data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
        $$data .= "  _DECLARE_CCTK_ARGUMENTS; \\\n";
        $$data .= "  CCTK_Checked_called(); \\\n";
        for my $th (keys %{$reads_writes->{$namekey}}) {
          for my $full_var (keys %{$reads_writes->{$namekey}->{$th}}) {
            my $var_group;
            my $group_register;
            my $timelevel = 0;
            my $var = $full_var;
            my $timelevel = 0;
            while((substr $var,-2,2) eq "_p") {
              # This loop determines the timelevel by tallying the
              # timelevel suffixes '_p' and removing them from the
              # variable name.
              $var = substr $var,0,-2;
              $timelevel++;
            }
            if(defined($hash->{$th}->{variable_list}->{$var})) {
              # public variables
              my $group = $hash->{$th}->{variable_list}->{$var};
              $var_group = $hash->{$th}->{$group};
            } elsif(($tnm eq $th) && defined($hash->{private_variable}->{$th}->{variable_list}->{$var})) {
              # private variables
              my $group = $hash->{private_variable}->{$th}->{variable_list}->{$var};
              $var_group = $hash->{private_variable}->{$th}->{$group};
            } elsif(defined($hash->{$th}->{$var})) {
              # variable name is actually a group
              $var_group = $hash->{$th}->{$var};
              $group_register = "yes";
            } else {
              # We need the write directive in the schedule.ccl to
              # match the case of the corresponding declaration in
              # the interface.ccl. If it doesn't line up, an error
              # will occur. This helps the user figure it out.
              my $hint = "";
              for my $v (%{$hash->{$th}->{variable_list}}) {
                if(lc $v eq lc $var) {
                    $hint = "Did you mean ${th}::$v?";
                }
              }
              if($hint eq "" and $tnm eq $th) {
                for my $v (%{$hash->{private_variable}->{$th}->{variable_list}}) {
                  if(lc $v eq lc $var) {
                      $hint = "Did you mean ${th}::$v?";
                  }
                }
              }
              &CST_error(0, "Error in $nm schedule. Check variable or group ${th}::$full_var" .
                    ' and verify correct implementation/thorn name and variable name.'
                    ,$hint, , __LINE__, __FILE__);
            }
            my $vtype = "CCTK_".$var_group->{vtype};
            if($vtype eq "CCTK_") {
              print "vtype='$vtype'\n";
              print Dumper($var_group),"\n";
              die "Bad variable group";
            }
            my $const = "";
            $const = "const" if($reads_writes->{$namekey}->{$th}->{$full_var}==0);
            if($group_register eq "yes") {
              for my $variables (keys %{$var_group->{grp_vars}}) {
                my $vname = "${th}::$variables";
                if ($var_group->{vector} ne "0") {
                  $vname .= "[0]";
                }
                $$data .= qq(  $const $vtype *$variables = ($const $vtype *)CCTK_VarDataPtr(cctkGH, 0, "$vname"); \\\n);
              }
            } else {
              my $vname = "${th}::$var";
              if ($var_group->{vector} ne "0") {
                $vname .= "[0]";
              }
              $$data .= qq(  $const $vtype *$full_var = ($const $vtype *)CCTK_VarDataPtr(cctkGH, $timelevel, "$vname"); \\\n);
            }
          } # loop over read/write variables
        } # loop over read/write thorns
      } elsif($lang->{$namekey} eq "FORTRAN") {
        my $vector_len = {};
        $$data .= "#ifdef FCODE \n";
        $$data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
        $$data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
        $$data .= "  _DECLARE_CCTK_FARGUMENTS \\\n";
        for my $th (keys %{$reads_writes->{$namekey}}) {
          for my $full_var (keys %{$reads_writes->{$namekey}->{$th}}) {
            my $var_group;
            my $group;
            my $group_register;
            my $var = $full_var;
            my $timelevel = 0;
            while((substr $var,-2,2) eq "_p") {
              # This loop determines the timelevel by tallying the
              # timelevel suffixes '_p' and removing them from the
              # variable name.
              $var = substr $var,0,-2;
              $timelevel++;
            }
            if(defined($hash->{$th}->{variable_list}->{$var})) {
              # public variables
              $group = $hash->{$th}->{variable_list}->{$var};
              $var_group = $hash->{$th}->{$group};
            } elsif(($tnm eq $th) && defined($hash->{private_variable}->{$th}->{variable_list}->{$var})) {
              # private variables
              $group = $hash->{private_variable}->{$th}->{variable_list}->{$var};
              $var_group = $hash->{private_variable}->{$th}->{$group};
            } elsif(defined($hash->{$th}->{$var})) {
              # variable name is actually a group
              $group = $var;
              $var_group = $hash->{$th}->{$var};
              $group_register = "yes";
            } else {
              # We need the write directive in the schedule.ccl to
              # match the case of the corresponding declaration in
              # the interface.ccl. If it doesn't line up, an error
              # will occur. This helps the user figure it out.
              my $hint = "";
              for my $v (%{$hash->{$th}->{variable_list}}) {
                if(lc $v eq lc $var) {
                    $hint = "Did you mean ${th}::$v?";
                }
              }
              if($hint eq "" and $tnm eq $th) {
                for my $v (%{$hash->{private_variable}->{$th}->{variable_list}}) {
                  if(lc $v eq lc $var) {
                      $hint = "Did you mean ${th}::$v?";
                  }
                }
              }
              &CST_error(0, "Error in $nm schedule. Check variable or group ${th}::$full_var" .
                    ' and verify correct implementation/thorn name and variable name.'
                    ,$hint, , __LINE__, __FILE__);
            }
            my $vtype = "CCTK_".$var_group->{vtype};
            $vtype .= ", intent(in)" if($reads_writes->{$namekey}->{$th}->{$full_var}==0);
            my $arrays = "";
            # The following logic determines the correct
            # indexing for the Fortran arrays and adds the
            # index variables to the macro.
            if($var_group->{gtype} eq "GF") {
              if($var_group->{vector} ne "0") {
                my $glen = $group."_length";
                if(!defined($vector_len->{$glen})) {
                  $temp_data .= ", $glen";
                  $$data .= "  integer :: $glen &&\\\n";
                  $vector_len->{$glen} = 1;
                }
                $arrays = qq((cctk_ash1,cctk_ash2,cctk_ash3,$glen));
              } else {
                $arrays = qq((cctk_ash1,cctk_ash2,cctk_ash3));
              }
            } elsif($var_group->{gtype} eq "ARRAY") {
              #if($var_group->{vector} ne "0") {
                #I haven't seen a vector-array in fortran yet, so I don't know
                #the proper order for the arguments.
              #}
              my $glen = "X0".$group;
              if(!defined($vector_len->{$glen})) {
                $temp_data .= ", $glen";
                $$data .= "  integer :: $glen &&\\\n";
                $vector_len->{$glen} = 1;
              }
              for(my $i = 1; $i < $var_group->{array_dim}; $i++) {
                my $temp_glen .= "X".$i.$group;
                $glen .= ",".$temp_glen;
                if(!defined($vector_len->{$temp_glen})) {
                  $temp_data .= ", $temp_glen";
                  $$data .= "  integer :: $temp_glen &&\\\n";
                  $vector_len->{$temp_glen} = 1;
                }
              }
              $arrays = qq(($glen));
            } elsif($var_group->{vector} ne "0") {
              my $glen = $group."_length";
              $temp_data .= ", $glen";
              $$data .= "  integer :: $glen &&\\\n";
              $arrays = qq(($glen));
            }
            if($group_register eq "yes") {
              for my $variables (keys %{$var_group->{grp_vars}}) {
                $temp_data .= ", $variables";
                $$data .= "  $vtype :: $variables $arrays &&\\\n";
                $$data .= "  integer, parameter :: cctki_use_$variables = kind($variables) &&\\\n";
              }
            } else {
              $temp_data .= ", $full_var";
              $$data .= "  $vtype :: $full_var $arrays &&\\\n";
              $$data .= "  integer, parameter :: cctki_use_$full_var = kind($full_var) &&\\\n";
            }
          } # loop over read/write variables
        } # loop over read/write thorns
      } else {
        &CST_error(0, "Failed to match the language for the function $nm."
            ,"", __LINE__, __FILE__);
      }
      $$data .= "  /* end $nm */\n";
      $$data .= "#endif\n";
      if($lang->{$namekey} eq "FORTRAN") {
        $$data .= "#ifndef CCTK_ARGUMENTS_${nm} \n";
        $$data .= "#define CCTK_ARGUMENTS_$nm _CCTK_ARGUMENTS$temp_data \n";
        $$data .= "#endif\n";
      }
      $$data .= "#endif\n";
    } # if logic for empty/non-empty macros
  } #loop over functions
  $$data .= "#endif";
}

sub GenerateArguments
{
  my %thorns = @_;
  my $hash = {};
  my $ccl_file;
  for my $key (keys %thorns) {
    $ccl_file = $thorns{$key}."/interface.ccl";
    my $gr=parse_ccl($I_grammar,$I_rule,$ccl_file,$int_file);
    if($gr) {
      interface_starter($key,$hash,$gr,$ccl_file);
    }
  }
  for my $key (keys %thorns) {
    open(my $fh, '>', $ENV{CCTK_HOME}."/configs/sim/bindings/include/$key/cctk_Arguments_Checked.h");
    $ccl_file = $thorns{$key}."/schedule.ccl";
    my $gr=parse_ccl($S_grammar,$S_rule,$ccl_file,$sch_file);
    if($gr) {
      print $fh "#ifdef CCODE\n";
      print $fh "extern\n";
      print $fh "#ifdef __cplusplus\n";
      print $fh "\"C\"\n";
      print $fh "#endif\n";
      print $fh "void CCTK_Checked_called();\n";
      print $fh "#endif\n";
      print $fh schedule_starter($key,$hash,$gr,$ccl_file);
    }
    close($fh);
  }
}
