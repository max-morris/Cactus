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
          my $name = $gch->has(0,"name");
          my $name_str = uc $name->substring();
          $hash->{$name_str} = {};
          do_interfaces($hash->{$name_str},$gr);
          $hash->{$thornname}->{$thornname} = $hash->{$name_str};
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
            confess("This parser has met an unexpected structure in interface parsing.");
          }
        }
      } elsif($ch->is("gtype")) {
        $gtype = $ch->substring();
      } elsif($ch->is("timelevels")) {
        $level = $ch->substring();
        last;
      }
    }
    $hash->{$gname}->{"level"} = [ 0 => ($level-1) ] -> [ 0 <= ($level-1) ];
    $hash->{$gname}->{"vtype"} = uc $vtype;
    $hash->{$gname}->{"vector"} = $vecval;
    $hash->{$gname}->{"gtype"} = $gtype;
    my $noDetect = 0;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("VARS")) {
        my $i = 0;
        $noDetect = 1;
        while($ch->has($i,"name")!=0) {
          my $var = $ch->has($i,"name")->substring();
          $hash->{$gname}->{"grp_vars"}->{$var} = $var;
          $hash->{"variable_list"}->{$var} = $gname;
          $i++;
        }
        last;
      }
    }
    if($noDetect == 0) {
      $hash->{$gname}->{"grp_vars"}->{$gname} = $gname;
      $hash->{"variable_list"}->{$gname} = $gname;
    }
  } else {
    for my $ch (@{$gr->{children}}) {
      do_interfaces($hash,$ch);
    }
  }
}

sub do_schedules
{
   my $data;
   my $tnm = uc shift;
   my $hash = shift;
   my $gr = shift;
   if($gr->is("schedule")) {
     my $nm;
     next if($gr->has(0,"group")); #group scheduling has no rd/wr clauses
     my $language;
     my $reads_writes = {};
     for my $ch (@{$gr->{children}}) {
       if($ch->is("name")) {
         $nm = $ch->substring();
       } elsif($ch->is("lang")) {
         $language = uc $ch->has(0,"name")->substring();
       } elsif($ch->is("reads") or $ch->is("writes")) {
         my $is_writes = $ch->is("writes");
         my $vname = $ch->has(0,"qname")->has(0,"vname");
         my $thorn = uc $vname->has(0,"name")->substring();
         my $var = $vname->has(1,"name")->substring();
         $reads_writes->{$thorn}->{$var} += $is_writes;
         my $i = 1;
         while($ch->has($i,"qname")) {
           $vname = $ch->has($i,"qname")->has(0,"vname");
           $var = $vname->has(0,"name")->substring();
           $reads_writes->{$thorn}->{$var} += $is_writes;
            $i++;
         }
       }
     }
     my $temp_data = "";
     $data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
     $data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
     if($language eq "C") {
       $data .= " _DECLARE_CCTK_ARGUMENTS; \\\n";
       for my $th (keys %{$reads_writes}) {
         for my $full_var (keys %{$reads_writes->{$th}}) {
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
             warn("Variable or group $th::$full_var not found. Error in $nm schedule.");
             next;
           }
           my $vtype = "CCTK_".$var_group->{"vtype"};
           my $const = "";
           $const = "const" if($reads_writes->{$th}->{$full_var}==0);
           if($group_register eq "yes") {
             for my $variables (keys %{$var_group->{"grp_vars"}}) {
               my $vname = "$th::$variables";
               if ($var_group->{"vector"} ne "0") {
                 $vname .= "[0]";
               }
               $data .= qq(  $const $vtype *$variables = ($const $vtype *)CCTK_VarDataPtr(cctkGH, 0, "$vname"); \\\n);
             }
           } else {
             my $vname = "$th::$var";
             if ($var_group->{"vector"} ne "0") {
               $vname .= "[0]";
             }
             $data .= qq(  $const $vtype *$full_var = ($const $vtype *)CCTK_VarDataPtr(cctkGH, $timelevel, "$vname"); \\\n);
           }
         }
       }
     } elsif($language eq "FORTRAN") {
       $data .= " _DECLARE_CCTK_ARGUMENTS \\\n";
       for my $th (keys %{$reads_writes}) {
         for my $full_var (keys %{$reads_writes->{$th}}) {
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
             confess("Variable $th::$var not found. Error in $nm schedule.");
           }
           my $vtype = "CCTK_".$var_group->{"vtype"};
           $vtype .= ", intent(in)" if($reads_writes->{$th}->{$full_var}==0);
           my $arrays = "";
           if($var_group->{"gtype"} eq "GF") {
             if($var_group->{"vector"} ne "0") {
               my $glen = $group."_length";
               $arrays = qq((cctk_ash1,cctk_ash2,cctk_ash3,$glen));
             } else {
               $arrays = qq((cctk_ash1,cctk_ash2,cctk_ash3));
             }
           } elsif($var_group->{"vector"} ne "0") {
             my $glen = $group."_length";
             $arrays = qq(($glen));
           }
           if($group_register eq "yes") {
             for my $variables (keys %{$var_group->{"grp_vars"}}) {
               $temp_data .= ", $variables";
               $data .= "  $vtype :: $variables $arrays &&\\\n";
               $data .= "  integer, parameter :: cctki_use_$variables = kind($variables) &&\\\n";
             }
           } else {
             $temp_data .= ", $full_var";
             $data .= "  $vtype :: $full_var $arrays &&\\\n";
             $data .= "  integer, parameter :: cctki_use_$full_var = kind($full_var) &&\\\n";
           }
         }
       }
     } else {
       confess("rdwr.pl failed to match the language for the function $nm.");
     }
     $data .= " /* end $nm */\n";
     $data .= "#endif\n";
     $data .= "#ifndef CCTK_ARGUMENTS_${nm} \n";
     $data .= "#define CCTK_ARGUMENTS_$nm _CCTK_FARGUMENTS$temp_data \n";
     $data .= "#endif\n";
   } else {
     for my $ch (@{$gr->{children}}) {
       $data .= do_schedules($tnm,$hash,$ch);
     }
   }
   return $data;
}
sub GenerateArguments
{
  my %thorns = @_;
  my $hash = {};
  for my $key (keys %thorns) {
    my $p=piraha::parse_src($I_grammar,$I_rule,"$thorns{$key}/interface.ccl");
    my $m = $p->matches();
    if($m) {
      my $gr = $p->{gr};
      interface_starter($key,$hash,$gr);
    } else {
      print "CST ERROR IN FILE  ";
      $p->showError();
      confess("Parse Error");
    }
  }
  for my $key (keys %thorns) {
    open(my $fh, '>', $ENV{CCTK_HOME}."/configs/sim/bindings/include/$key/cctk_Arguments_$key.h");
    my $p=piraha::parse_src($S_grammar,$S_rule,"$thorns{$key}/schedule.ccl");
    my $m = $p->matches();
    if($m) {
      my $gr = $p->{gr};
      print $fh do_schedules($key,$hash,$gr);
    } else {
      print "CST ERROR IN FILE  ";
      $p->showError();
      confess("Parse Error");
    }
    close($fh);
  }
}
