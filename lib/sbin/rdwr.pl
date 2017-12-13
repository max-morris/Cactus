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
  for my $ch (@{$gr->{children}}) {
    if($ch->is("FUNC_GROUP")) {
      for my $gch (@{$ch->{children}}) {
        if($gch->is("IMPLEMENTS")) {
          my $name = uc $gch->has(0,"name")->substring();
          $hash->{$name} = {} if(!defined($hash->{$name}));
          do_interfaces($hash->{$name},$gr);
          $hash->{$thornname}->{$thornname} = $hash->{$name};
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
  if($gr->is("GROUP_VARS")) {
    my $vtype = $gr->has(0,"vtype")->substring();
    my $level = 0;
    my $vecval = "0";
    my $gname;
    my $gtype;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("gname")) {
        $gname = $ch->has(0,"name")->substring();
        if($ch->has(1,"expr")) {
          my $expr = $ch->has(1,"expr")->has(0,"addexpr")->has(0,"mulexpr")->has(0,"powexpr");
          if($expr->has(0,"num")) {
            $vecval = $expr->has(0,"num")->substring();
          } elsif($expr->has(0,"accname")) {
            $vecval = $expr->has(0,"accname")->substring();
          } else {
            &CST_error(0, "Unexpected structure encountered in interface parsing."
                  , __LINE__, __FILE__);
          }
        }
      } elsif($ch->is("gtype")) {
        $gtype = $ch->substring();
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
    $hash->{$gname}->{gtype} = $gtype;
    my $Detect = 0;
    for my $ch (@{$gr->{children}}) {
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
      $hash->{$gname}->{grp_vars}->{$gname} = $gname;
      $hash->{variable_list}->{$gname} = $gname;
    }
  } else {
    for my $ch (@{$gr->{children}}) {
      do_interfaces($hash,$ch);
    }
  }
}

sub schedule_starter
{
  my $tnm = uc shift;
  my $hash = shift;
  my $gr = shift;
  my $lang = {};
  my $reads_writes = {};
  my $data = "";
  do_schedules($gr,$reads_writes,$lang);
  create_macros($tnm,$hash,$gr,$reads_writes,$lang,\$data);
  return $data;
}

sub do_schedules
{
  my $gr = shift;
  my $reads_writes = shift;
  my $lang = shift;
  if($gr->is("schedule")) {
    next if($gr->has(0,"group")); #group scheduling has no rd/wr clauses
    my $nm;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("name")) {
        $nm = $ch->substring();
      } elsif($ch->is("lang")) {
        $lang->{$nm} = uc $ch->has(0,"name")->substring();
      } elsif($ch->is("reads") or $ch->is("writes")) {
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
      }
    }
    if(!defined($reads_writes->{$nm})) {
      $reads_writes->{$nm} = "empty";
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
  for my $nm (keys %{$reads_writes}) {
    my $temp_data = "";
    $$data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
    $$data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
    if($reads_writes->{$nm} eq "empty") {
      if ($lang->{$nm} eq "C") {
        $$data .= " _DECLARE_CCTK_ARGUMENTS; \\\n";
      } elsif ($lang->{$nm} eq "FORTRAN") {
        $$data .= " _DECLARE_CCTK_FARGUMENTS; \\\n";
      } else {
        &CST_error(0, "Failed to match the language for the function $nm."
              , __LINE__, __FILE__);
      }
      $$data .= " /* end $nm */\n";
      $$data .= "#endif\n";
      $$data .= "#ifndef CCTK_ARGUMENTS_${nm} \n";
      $$data .= "#define CCTK_ARGUMENTS_$nm _CCTK_ARGUMENTS \n";
      $$data .= "#endif\n";
    } else {
      if($lang->{$nm} eq "C") {
        $$data .= " _DECLARE_CCTK_ARGUMENTS; \\\n";
        for my $th (keys %{$reads_writes->{$nm}}) {
          for my $full_var (keys %{$reads_writes->{$nm}->{$th}}) {
            my $var_group;
            my $group_register;
            my $timelevel = 0;
            my $var = $full_var;
            my $timelevel = 0;
            while((substr $var,-2,2) eq "_p") {
              $var = substr $var,0,-2;
              $timelevel++;
            }
            if(defined($hash->{$th}->{"variable_list"}->{$var})) {
              my $group = $hash->{$th}->{"variable_list"}->{$var};
              $var_group = $hash->{$th}->{$group};
            } elsif(($tnm eq $th) && defined($hash->{$th}->{$th}->{"variable_list"}->{$var})) {
              my $group = $hash->{$th}->{$th}->{"variable_list"}->{$var};
              $var_group = $hash->{$th}->{$th}->{$group};
            } elsif(defined($hash->{$th}->{$var})) {
              $var_group = $hash->{$th}->{$var};
              $group_register = "yes";
            } else {
              &CST_error(0, "Error in $nm schedule. Check variable or group $th::$full_var" .
                    ' and verify correct implementation/thorn name and variable name.'
                    , __LINE__, __FILE__);
            }
            my $vtype = "CCTK_".$var_group->{"vtype"};
            my $const = "";
            $const = "const" if($reads_writes->{$nm}->{$th}->{$full_var}==0);
            if($group_register eq "yes") {
              for my $variables (keys %{$var_group->{"grp_vars"}}) {
                my $vname = "$th::$variables";
                if ($var_group->{"vector"} ne "0") {
                  $vname .= "[0]";
                }
                $$data .= qq(  $const $vtype *$variables = ($const $vtype *)CCTK_PSVarDataPtr(cctkGH, 0, "$vname"); \\\n);
              }
            } else {
              my $vname = "$th::$var";
              if ($var_group->{"vector"} ne "0") {
                $vname .= "[0]";
              }
              $$data .= qq(  $const $vtype *$full_var = ($const $vtype *)CCTK_PSVarDataPtr(cctkGH, $timelevel, "$vname"); \\\n);
            }
          }
        }
      } elsif($lang->{$nm} eq "FORTRAN") {
        my $vector_len = {};
        $$data .= " _DECLARE_CCTK_FARGUMENTS \\\n";
        for my $th (keys %{$reads_writes->{$nm}}) {
          for my $full_var (keys %{$reads_writes->{$nm}->{$th}}) {
            my $var_group;
            my $group;
            my $group_register;
            my $var = $full_var;
            my $timelevel = 0;
            while((substr $var,-2,2) eq "_p") {
              $var = substr $var,0,-2;
              $timelevel++;
            }
            if(defined($hash->{$th}->{"variable_list"}->{$var})) {
              $group = $hash->{$th}->{"variable_list"}->{$var};
              $var_group = $hash->{$th}->{$group};
            } elsif(($tnm eq $th) && defined($hash->{$th}->{$th}->{"variable_list"}->{$var})) {
              $group = $hash->{$th}->{$th}->{"variable_list"}->{$var};
              $var_group = $hash->{$th}->{$th}->{$group};
            } elsif(defined($hash->{$th}->{$var})) {
              $group = $var;
              $var_group = $hash->{$th}->{$var};
              $group_register = "yes";
            } else {
              &CST_error(0, "Error in $nm schedule. Check variable or group $th::$full_var" .
                    ' and verify correct implementation/thorn name and variable name.'
                    , __LINE__, __FILE__);
            }
            my $vtype = "CCTK_".$var_group->{"vtype"};
            $vtype .= ", intent(in)" if($reads_writes->{$nm}->{$th}->{$full_var}==0);
            my $arrays = "";
            if($var_group->{"gtype"} eq "GF") {
              if($var_group->{"vector"} ne "0") {
                my $glen = $group."_length";
                if(!defined($vector_len->{$glen})) {
                  $temp_data .= ", $glen";
                  $$data .= "integer :: $glen &&\\\n";
                  $vector_len->{$glen} = 1;
                }
                $arrays = qq((cctk_ash1,cctk_ash2,cctk_ash3,$glen));
              } else {
                $arrays = qq((cctk_ash1,cctk_ash2,cctk_ash3));
              }
            } elsif($var_group->{"gtype"} eq "ARRAY") {
              my $glen = "X0".$group;
              if(!defined($vector_len->{$glen})) {
                $temp_data .= ", $glen";
                $$data .= "integer :: $glen &&\\\n";
                $vector_len->{$glen} = 1;
              }
              $arrays = qq(($glen));
            } elsif($var_group->{"vector"} ne "0") {
              my $glen = $group."_length";
              $temp_data .= ", $glen";
              $$data .= "integer :: $glen &&\\\n";
              $arrays = qq(($glen));
            }
            if($group_register eq "yes") {
              for my $variables (keys %{$var_group->{"grp_vars"}}) {
                $temp_data .= ", $variables";
                $$data .= "  $vtype :: $variables $arrays &&\\\n";
                $$data .= "  integer, parameter :: cctki_use_$variables = kind($variables) &&\\\n";
              }
            } else {
              $temp_data .= ", $full_var";
              $$data .= "  $vtype :: $full_var $arrays &&\\\n";
              $$data .= "  integer, parameter :: cctki_use_$full_var = kind($full_var) &&\\\n";
            }
          }
        }
      } else {
        &CST_error(0, "Failed to match the language for the function $nm."
              , __LINE__, __FILE__);
      }
      $$data .= " /* end $nm */\n";
      $$data .= "#endif\n";
      $$data .= "#ifndef CCTK_ARGUMENTS_${nm} \n";
      $$data .= "#define CCTK_ARGUMENTS_$nm _CCTK_ARGUMENTS$temp_data \n";
      $$data .= "#endif\n";
    }
  } #loop over $nm
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
      interface_starter($key,$hash,$gr);
    }
  }
  for my $key (keys %thorns) {
    open(my $fh, '>', $ENV{CCTK_HOME}."/configs/sim/bindings/include/$key/cctk_Arguments_$key.h");
    $ccl_file = $thorns{$key}."/schedule.ccl";
    my $gr=parse_ccl($S_grammar,$S_rule,$ccl_file,$sch_file);
    if($gr) {
      print $fh schedule_starter($key,$hash,$gr);
    }
    close($fh);
  }
}
