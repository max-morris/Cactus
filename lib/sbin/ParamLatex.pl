#! /usr/bin/perl -s

$sbin_dir = "lib/sbin";

require "$sbin_dir/parameter_parser.pl";

$thorn{"WaveToy"} = "packages/CactusApplications/WaveToy";
%parameter_database = &create_parameter_database(%thorn);


  &latex_database(%parameter_database);


#/*@@
#  @routine    print_parameter_database
#  @date       Wed Sep 16 14:58:52 1998
#  @author     Tom Goodale
#  @desc 
#  Prints out a parameter database.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/
sub latex_database
{
  local(%parameter_database) = @_;
  local($field);
  
  $nvars = 0;

  foreach $field ( sort keys %parameter_database )
  {
    print "$field: $parameter_database{$field}\n";
    $field =~ /(.*)\s(.*)\s(.*)/;
    if ($field =~ /default/)
    {
      $default = $parameter_database{$field};
    }
    elsif ($field =~ /description/)
    {
      $description = $parameter_database{$field};
    }
    elsif ($field =~ /type/)
    {
        $type = $parameter_database{$field};
	$field =~ /\w*\s(\w*)\stype/;
	$name = $1;
	print "Variable $name has default $default\n";
	print "  -> $description\n";
	$variable[nvars] = $name;
        $vars++;
    }
  }
}
