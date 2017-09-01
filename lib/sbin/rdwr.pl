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
#    $hash->{$gname}->{"level"} = [ 0 => ($level-1) ] -> [ 0 <= ($level-1) ];
#    $hash->{$gname}->{"vtype"} = uc $vtype;
#    $hash->{$gname}->{"vector"} = $vecval;
#    $hash->{$gname}->{"gtype"} = $gtype;
    my $noDetect = 0;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("VARS")) {
        my $i = 0;
	$noDetect = 1;
	while($ch->has($i,"name")!=0) {
          my $var = $ch->has($i,"name")->substring();
	  $hash->{$gname}->{$var}->{"level"} = [ 0 => ($level-1) ] -> [ 0 <= ($level-1) ];
	  $hash->{$gname}->{$var}->{"vtype"} = uc $vtype;
	  $hash->{$gname}->{$var}->{"vector"} = $vecval;
	  $hash->{"variable_list"}->{$var} = $hash->{$gname}->{$var};
	  $hash->{"variable_list"}->{$var}->{"group"} = $gname;
	  $i++;
	}
        last;
      }
    }
    if($noDetect == 0) {
      $hash->{$gname}->{$gname}->{"level"} = [ 0 => ($level-1) ] -> [ 0 <= ($level-1) ];
      $hash->{$gname}->{$gname}->{"vtype"} = uc $vtype;
      $hash->{$gname}->{$gname}->{"vector"} = $vecval;
      $hash->{"variable_list"}->{$gname} = $hash->{$gname}->{$gname};
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
       }
     }
     $data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
     $data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
     $data .= " _DECLARE_CCTK_ARGUMENTS; \\\n";
     if($language eq "C") {
       for my $th (keys %{$reads_writes}) {
         for my $var (keys %{$reads_writes->{$th}}) {
	   my $variable;
	   my $group;
           if(defined($hash->{$th}->{$var})) {
	     $variable = $hash->{$th}->{$var};
           } elsif(($tnm eq $th) && defined($hash->{$tnm}->{$tnm}->{$var})) {
	     $variable = $hash->{$tnm}->{$tnm}->{$var};
	   } elsif(defined->) {
	   }
	   } else {
	     confess("Variable $th::$var not found. Error in $nm schedule.");
	   }
           my $vtype = "CCTK_".$variable->{"vtype"};
	   my $vname = "$th::$var";
	   if ($variable->{"vector"}) {
             $vname .= "[0]";
	   }
	   my $const = "";
	   $const = "const" if($reads_writes->{$th}->{$var}==0);
	   my $past = "";
	   for(my $i=0; $i<=$variable->{"level"}; $i++) {
	     my $tvar = $var.$past;
	     $data .= qq(  $const $vtype *$tvar = ($const $vtype *)CCTK_VarDataPtr(cctkGH, $i, "$vname"); \\\n);
	     $past .= "_p";
	   }
         }
       }
     } elsif($language eq "FORTRAN") {
       for my $th (keys %{$reads_writes}) {
         for my $var (keys %{$reads_writes->{$th}}) {
	   my $variable;
	   if(defined($hash->{$th}->{$var})) {
	     $variable = $hash->{$th}->{$var};
	   } elsif(($tnm eq $th) && defined($hash->{$tnm}->{$tnm}->{$var})) {
	     $variable = $hash->{$tnm}->{$tnm}->{$var};
	   } else {
	     confess("Variable $th::$var not found. Error in $nm schedule.");
	   }
           my $vtype = "CCTK_".$variable->{"vtype"};
	   my $vname = "$th::$var";
	   my $arrays = "";
	   $arrays .= "(cctk_ash1,cctk_ash2,cctk_ash3" if ($variable->{"type"} eq "GF");
#	   if() {
#	     $arrays .= "$variable->{"vector"}"
#	   } else {
#	     $arrays .= ")";
#	   }
	   $vtype .= ", intent(in)" if($reads_writes->{$th}->{$var}==0);
	   my $past = "";
	   for(my $i=0; $i<=$variable->{"level"}; $i++) {
	     my $tvar = $var.$past;
#	     $data .= "  $vtype :: $tvar $arrays"
#	     $data .= "  integer, parameter :: cctki_use_$tvar = kind($tvar)"
	     $past .= "_p";
	   }
	 }
       }
     } else {
       confess("rdwr.pl failed to match the language for the function $nm.");
     }
     $data .= " /* end $nm */\n";
     $data .= "#endif\n";
   } else {
     for my $ch (@{$gr->{children}}) {
       $data .= do_schedules($tnm,$hash,$ch);
     }
   }
   return $data;
}

#GF:  $vtype $tvar (cctk_ash1,cctk_ash2,cctk_ash3)
#     integer, parameter :: cctki_use_$tvar = kind($tvar)

#SCALAR:  $vtype $tvar
#         integer, parameter :: cctki_use_$tvar = kind($tvar)

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
  die;
}
