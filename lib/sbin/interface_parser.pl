#! /usr/bin/perl

#require "parameter_parser.pl";


#%implementations = ("flesh", "flesh", "test1", "test1", "test2", "test2");

#%interface_database = create_interface_database(%implementations);

#&print_interface_database(%interface_database);

#/*@@
#  @routine    create_interface_database
#  @date       Wed Sep 16 15:07:11 1998
#  @author     Tom Goodale
#  @desc 
#  Creates a database of all the interfaces
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub create_interface_database
{
    local(%thorns) = @_;
    local($thorn, @indata);
    local(@new_interface_data);
    local(@interface_data);

#  Loop through each  thorn's interface file.
    foreach $thorn (keys %thorns)
    {
#       Read the data
	@indata = read_file("$thorns{$thorn}/interface.ccl");
	
#       Get the interface data from it
	@new_interface_data = &parse_interface_ccl($thorn, @indata);

#       Add the interface to the master interface database
	push (@interface_data, @new_interface_data);

    }

    return @interface_data;
}


#/*@@
#  @routine    parse_interface_ccl
#  @date       Wed Sep 16 15:07:11 1998
#  @author     Tom Goodale
#  @desc 
#  Parses an interface.ccl file and generates a database of the values.
#  @enddesc 
#  @calls     
#  @calledby   
#  @history 
#
#  @endhistory 
#@@*/

sub parse_interface_ccl
{
    local($thorn, @data) = @_;
    local($linenum, $line, $block, $type, $variable, $description, $nerrors);
    local($current_friend, $new_ranges, $new_desc);
    local($data, %interface_db);
    local($implementation);

#   The default block is private.
    $block = "PRIVATE";

    for($linenum = 0; $linenum < @data; $linenum++)
    {
	$line = $data[$linenum];

#       Parse the line
	if($line =~ m/^\s*(PUBLIC|PROTECTED|PRIVATE)\s*$/i)
	{
#           It's a new block.
	    $block = "\U$1\E";
	} 
	elsif ($line =~ m/^\s*IMPLEMENTS:\s*([a-z]+[a-z_0-9]*)\s*$/i)
	{
	    if($implementation == 0)
	    {
		$implementation = $1;
		$interface_db{"\U$thorn\E IMPLEMENTS"} = $implementation;
	    }
	    else
	    {
		print "Error:  Only one implements line allowed.\n";
	    }
	}
	elsif ($line =~ m/^\s*(INHERITS|FRIEND)\s*:((\s*[a-z]+[a-z_0-9]*)*\s*)$/i)
	{
	    $interface_db{"\U$thorn $1\E"} .= $2;
	}
	elsif ($line =~ m/^\s*(PUBLIC|PROTECTED|PRIVATE)\s*:\s*$/i)
	{
	    $block = "\U$1\E";
	}
	elsif ($line =~ m/^\s*GROUP\s*([a-z]+[a-z_0-9]+)\s*$/i)
	{
	    $current_group = $1;

	    $interface_db{"\U$thorn $block GROUPS \E"} .= " $1";

	    $linenum++;
	    if($data[$linenum] =~ m/^\s*\{\s*$/)
	    {
		$linenum++;
		while($data[$linenum] =~ m/^\s*GF\s*\(([a-z]+[a-z_0-9]*)((,.*)*)\)\s*$/i)
		{
		    $interface_db{"\U$thorn GROUP $current_group\E"} .= " $1";
		    $interface_db{"\U$thorn $1 TYPE\E"} = "GF";
		    $interface_db{"\U$thorn $1 DATA\E"} = $2;
		    $linenum++;
		}
		if(! $data[$linenum] =~ m/^\s*\}\s*/)
		{
		    print STDERR "Expected }, got $data[$linenum]\n";
		}
	    }
	    else
	    {
		print STDERR "Expected {, got $data[$linenum]\n";
	    }
	}
	elsif($line =~ m/^\s*(INTEGER|REAL)\s*\(([a-z]+[a-z_0-9]*)\)\s*$/i)
	{
	    $interface_db{"\U$thorn $block SCALARS\E"} .= " $2";
	    $interface_db{"\U$thorn $2 TYPE\E"} = "$1";
	}
	else
	{
	    if($line =~ m:\{:)
	    {
		print "...Skipping block with missing keyword....\n";
		$linenum++ until ($data[$linenum] =~ m:\}:);
	    }
	    else
	    {
		print "Unknown line $line!!!\n";
	    }
	}
    }

    return %interface_db;
}


sub print_interface_database
{
    local(%database) = @_;
    local($field);

    foreach $field ( sort keys %database ){
	print "$field has value $database{$field}\n";
    }
}

1;
