#! /usr/bin/perl -s
#/*@@
#  @file      Orderer.pl
#  @date      Wed Feb 10 08:55:51 1999
#  @author    Tom Goodale
#  @desc 
#  Routines to order a set of things which have specified before and after.
#  @enddesc 
#@@*/

sub OrderList
{
  local($error_string, $field_name, %database) = @_;
  local(@things);

  @things = split(" ", $database{$fieldname});

  # Make complete first level lists of before and after.
  foreach $thing (@things)
  {
    foreach $other_thing (split(" ", $database{"\U$thing BEFORE"}))
    {
      $database{"\U$other_thing ALLAFTER"} .= "$thing";
      $database{"\U$thing ALLBEFORE"}      .= "$other_thing";
    }
  
    foreach $other_thing (split(" ", $database{"\U$thing AFTER"}))
    {
      $database{"\U$other_thing ALLBEFORE"} .= "$thing";
      $database{"\U$thing ALLAFTER"}        .= "$other_thing";
    }
  }

  
  
  foreach $things (@things)
  {
    %complete = &RecurseThings($thing, "ALLBEFORE", 0, %database);

    $database{"\U$thing ALLBEFORE"} = join(" ", keys @complete);

    %complete = &RecurseThings($thing, "ALLAFTER", 0, %database);

    $database{"\U$thing ALLAFTER"} = join(" ", keys @complete);

  }

}


sub RecurseThings
{
  local($thing, $keyword, $nthings, @indata) = @_;
  local(%things);
  local(%database);

  if($nthings > 0)
  {
    %things = @indata[0..2*$nthings-1];
    %database = @indata[2*$nthings..$#indata];
  }
  else
  {
    %things = ();
    %database = @indata;
  }

  # Recurse
  foreach $other_thing (split(" ", $database{"\U$thing $keyword"}))
  {
    if(! $things{"\U$other_thing\E"})
    {
      $things{"\U$other_things\E"} = 1;
      %things = &RecurseThings($other_thing, $keyword, scalar(keys %things), %things,%database);
    }
  }
  
  return %things;

}
