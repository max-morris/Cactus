#!/usr/bin/perl -w

#/*@@
#  @file    f_depend_modules.pl
#  @author  Erik Schnetter
#  @date    19 January 2004
#  @desc
#           Create dependencies for Fortran 90 "use" and "include" statements
#  @enddesc
#  @version $Header$
# @@*/



use strict;

my $srcfile = $ARGV[0];
my $dest = $ARGV[1];
my $srcdir = $ARGV[2];
my @otherdirs = @ARGV[3..$#ARGV];

my @suffixes = (".f77", ".f", ".f90", ".F77", ".F", ".F90");

print "$dest:";

my %modules;

my $line = 0;
while (<STDIN>)
{
  ++ $line;
  if (/^\s*#\s*(\d+)/)
  {
    # line number directive from C preprocessor
    $line = $1 - 1;
  }
  elsif (/^\s*include\s*['"]([^'"]+)['"]/i)
  {
    # include statement
    my $name = $1;
    print " \\\n  $srcdir/$name";
  }
  elsif (/^\s*module\s+(\w+)/i)
  {
    # begin of a module
    my $name = $1;
    $modules{$name} = 1;
  }
  elsif (/^\s*use\s+(\w+)/i)
  {
    # use statement
    my $name = $1;
    my $found = 0;
    if (! $found)
    {
      # reference to a module in this file?
      if ($modules{$name})
      {
        $found = 1;
      }
    }
    if (! $found)
    {
      # reference to a module in this thorn?
      my $dirhdl;
      if( opendir( $dirhdl, "$srcdir" ) )
      {
        while( defined( my $filename = readdir( $dirhdl ) ) )
        {
          loop: foreach my $suffix (@suffixes)
          {
            if( $filename eq "$name$suffix" )
            {
              $found = 1;
              print " \\\n  $filename.o";
              last loop;
            }
          }
        }
        closedir $dirhdl;
      }
    }
    if (! $found)
    {
      # reference to a module in another thorn?
    loop: foreach my $dir (@otherdirs)
      {
        # note: we could also use the SUBDIRS from the make.code.defn here
        foreach my $subdir (".", "include")
        {
          my $dirhdl;
          if( opendir( $dirhdl, "$dir/$subdir" ) )
          {
            while( defined( my $filename = readdir( $dirhdl ) ) )
            {
              foreach my $suffix (@suffixes)
              {
                if( $filename eq "$name$suffix.o" )
                {
                  $found = 1;
                  print " \\\n  $dir/$subdir/$filename";
                  last loop;
                }
              }
            }
            closedir $dirhdl;
          }
        }
      }
    }
    if (! $found)
    {
      print STDERR "$srcfile:$line: Warning: While tracing module depencencies: Source file for module \"$name\" not found\n";
      if (@otherdirs)
      {
        print STDERR "   Searched in thorn directory and in [" . join(', ', @otherdirs) . "]\n";
      }
      else
      {
        print STDERR "   Searched in thorn directory only.\n";
      }
    }
  }
}

print "\n";
