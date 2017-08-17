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
    my $vtype = $gr->has(0,"vtype");
    my $level;
    my $noDetect = 0;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("timelevels")) {
	$noDetect = 1;
	$level = $ch->substring();
	last;
      }
    }
    $level = 0 if($noDetect == 0);
    $noDetect = 0;
    for my $ch (@{$gr->{children}}) {
      if($ch->is("VARS")) {
        my $i = 0;
	$noDetect = 1;
	my $vecval = 0;
        for my $ch2 (@{$gr->{children}}) {
          if($ch2->is("gname")) {
	    if($ch2->has(1,"expr")) {
	      $vecval = 1;
            }
	    last;
          }
	}
	while($ch->has($i,"name")!=0) {
          my $var = $ch->has($i,"name");
	  my $var_str = $var->substring();
	  $hash->{$var_str}->{"level"} = [ 0 => ($level-1) ] -> [ 0 <= ($level-1) ];
	  $hash->{$var_str}->{"vtype"} = uc $vtype->substring();
	  $hash->{$var_str}->{"vector"} = $vecval;
	  $i++;
	}
        last;
      }
    }
    if($noDetect == 0) {
      for my $ch (@{$gr->{children}}) {
        if($ch->is("gname")) {
          my $var = $ch->has(0,"name");
          my $var_str = $var->substring();
          $hash->{$var_str}->{"level"} = [ 0 => ($level-1) ] -> [ 0 <= ($level-1) ];
	  $hash->{$var_str}->{"vtype"} = uc $vtype->substring();
	  $hash->{$var_str}->{"vector"} = 0;
	  if($ch->has(1,"expr")) {
#	    my $vec = $ch->has(1,"expr")->has(0,"addexpr")->has(0,"mulexpr")->has(0,"powexpr")->has(0,"num");
#	    $hash->{$var_str}->{"vector"} = $vec->substring() if($vec);
	    $hash->{$var_str}->{"vector"} = 1;
	  }
	  last;
	}
      }
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
     my $reads_writes = {};
     for my $ch (@{$gr->{children}}) {
       if($ch->is("name")) {
         $nm = $ch->substring();
       } elsif($ch->is("reads") or $ch->is("writes")) {
         my $is_writes = $ch->is("writes");
         my $qname = $ch->has(0,"qname");
         my $vname = $qname->has(0,"vname");
         my $thorn = $vname->has(0,"name");
         my $var = $vname->has(1,"name");
         my $thorn_str = uc $thorn->substring();
         my $var_str = $var->substring();
         $reads_writes->{$thorn_str}->{$var_str} += $is_writes;
         my $val = $reads_writes->{$thorn_str}->{$var_str};
       }
     }
     $data .= "#ifndef DECLARE_CCTK_ARGUMENTS_${nm} \n";
     $data .= "#define DECLARE_CCTK_ARGUMENTS_${nm} \\\n";
     $data .= " _DECLARE_CCTK_ARGUMENTS; \\\n";
     for my $th (keys %{$reads_writes}) {
       for my $var (keys %{$reads_writes->{$th}}) {
         if(defined($hash->{$th}->{$var})) {
           my $vtype = "CCTK_".$hash->{$th}->{$var}->{"vtype"};
	   my $vname = "$th::$var";
	     if ($hash->{$th}->{$var}->{"vector"}) {
               $vname .= "[0]";
	     }
	   my $const = "";
	   $const = "const" if($reads_writes->{$th}->{$var}==0);
	   for(my $i=0; $i<=$hash->{$th}->{$var}->{"level"}; $i++) {
	     my $past = "";
	     for(my $lvls=1; $lvls<=$i; $lvls++) {
	       $past .= "_p";
	     }
	     my $tvar = $var.$past;
	     $data .= qq(  $const $vtype *$tvar = ($const $vtype *)CCTK_VarDataPtr(cctkGH, $i, "$vname"); \\\n);
	   }
	 } elsif(($tnm eq $th) && defined($hash->{$tnm}->{$tnm}->{$var})) {
           my $vtype = "CCTK_".$hash->{$tnm}->{$tnm}->{$var}->{"vtype"};
	   my $vname = "$tnm::$var";
	     if ($hash->{$tnm}->{$tnm}->{$var}->{"vector"}) {
	       $vname .= "[0]";
	     }
	   my $const = "";
	   $const = "const" if($reads_writes->{$th}->{$var}==0);
	   my $past = "";
	   for(my $i=0; $i<=$hash->{$tnm}->{$tnm}->{$var}->{"level"}; $i++) {
	     my $tvar = $var.$past;
	     $data .= qq(  $const $vtype *$tvar = ($const $vtype *)CCTK_VarDataPtr(cctkGH, $i, "$vname"); \\\n);
	     $past .= "_p";
	   }
	 } else {
	   confess("Variable $th::$var not found. Error in $nm schedule.");
	 }
       }
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
