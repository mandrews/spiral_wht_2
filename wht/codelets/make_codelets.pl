#!/usr/bin/env perl
#  make_codelets.pl
#
#  Generate codelets and register them in a C array and with the automake
#  system.
#
#  Author: Michael Andrews <mjand@drexel.edu>
#  Version: 
#  Date:
#

use vars qw/ %opt /;
use List::Util qw/ min /;
use Getopt::Std;
use File::Basename qw/ dirname /;

$opts = 'm:v:l:';
getopts($opts, \%opt);

$small      = $opt{m} || 6;
$vector     = $opt{v} || 2;
$interleave = $opt{l} || 4;

$path       = dirname($0);
$whtgen     = "$path/../../whtgen/whtgen";
$registry   = "$path/codelet_registry.h";

@codelets = ();
$codelets = 0;

# Greps through a codelet file for the function call
sub function_name {
  my $file = shift @_;
  my $call = "";
  open(FD, $file);
  while (<FD>) {
    next unless $_ =~ /\s*void\s*([\w\d]+)\s*\(.*?\)$/;
    $call = $1;
    last;
  }
  close(FD);
  return $call;
}

# Generate unaligned unrolled codelets
for ($i=1;$i<=$small;$i++) {
  push(@codelets,($i, "small",  [], "s_$i.c", "-n $i"));
  $codelets++;
}

# Generate aligned vectorized codelets
for ($k=1;$k<=$vector;$k++) {
  $v = 2**$k;
  for ($i=$k+1;$i<=$small;$i++) {
    push(@codelets,($i, "smallv", [ $v ], "s_$i\_v\_$v\_a.c", "-n $i -v $v -a"));
    $codelets++;
  }
}

# Generate unaligned interleaved codelets
for ($i=1;$i<=$small;$i++) {
  for ($j=1;$j<=$interleave;$j++) {
    $k = 2**$j;
    push(@codelets,($i, "smallil", [ $k ], "s_$i\_il\_$k.c", "-n $i -i $k"));
    $codelets++;
  }
}

# Generate unaligned interleaved and vectorized codelets
for ($i=1;$i<=$small;$i++) {
  for ($j=1;$j<=$interleave;$j++) {
    $l = 2**$j;
    for ($k=1;$k<=$vector;$k++) {
      $v = 2**$k;
      next unless $j >= $k; 
      push(@codelets,($i, "smallv", [ $v, $l ], "s_$i\_il\_$l\_v$v.c", "-n $i -i $l -v $v"));
      $codelets++;
    }
  }
}

$externs  = "";
$structs  = "";
$depends  = "";

while (@codelets) {
  $size     = shift @codelets;
  $name     = shift @codelets;
  $params   = shift @codelets;
  $file     = shift @codelets;
  $args     = shift @codelets;

  $n        = scalar(@$params);
  $params   = join(',', @$params) || '0';

  print "Generate:  $whtgen $args > $file\n";
  `$whtgen $args > $file`; # Generate codelet
  $call = function_name($file); # Grep for codelet function name

  print "Register: $name in $registry\n";

  $depends .= "$file "; # Add to dependancies
  $externs .= "extern codelet $call\;\n"; # Add to external declarations
  $structs .= "  { $size, \"$name\", 
    (codelet) &$call, 
    $n, { $params } }, \n"; # Add to registry struct
}

# Output the C registry array (see codelets.c)
open(FD, ">$registry");
print FD <<TEXT
#include "wht.h"

$externs

static const int wht_codelet_registry_size = $codelets;

static const codelet_entry
wht_codelet_registry[] =
{
$structs
};
TEXT
;
close(FD);

# Output the automake dependancies (see Makefile.am)
open(FD, ">.codelets");
print FD "CODELETS = $depends\n";
close(FD);

# vim: set ts=4 tw=79:

