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
  push(@codelets,("small[$i]",  "s_$i.c"));
}

# Generate aligned vectorized codelets
for ($k=1;$k<=$vector;$k++) {
  $v = 2**$k;
  for ($i=$k+1;$i<=$small;$i++) {
    push(@codelets,("smallv($v)[$i]",  "s_$i\_v\_$v\_a.c"));
  }
}

# Generate unaligned interleaved codelets
for ($i=1;$i<=$small;$i++) {
  for ($j=1;$j<=$interleave;$j++) {
    $k = 2**$j;
    push(@codelets,("smallil($k)[$i]", "s_$i\_il\_$k.c"));
  }
}

# Generate unaligned and aligned interleaved and vectorized codelets
for ($i=1;$i<=$small;$i++) {
  for ($j=1;$j<=$interleave;$j++) {
    $l = 2**$j;
    for ($k=1;$k<=$vector;$k++) {
      $v = 2**$k;
      next unless $j >= $k; 
      push(@codelets,("smallv($v,$l,0)[$i]", "s_$i\_il\_$l\_v\_$v.c"));
      push(@codelets,("smallv($v,$l,1)[$i]", "s_$i\_il\_$l\_v\_$v\_a.c"));
    }
  }
}

$externs  = "";
$structs  = "";
$depends  = "";

while (@codelets) {
  $name     = shift @codelets;
  $file     = shift @codelets;

  print "Generate:  $whtgen $name > $path/$file\n";
  `$whtgen '$name' > $path/$file`; # Generate codelet
  $call = function_name("$path/$file"); # Grep for codelet function name

  print "Register: $name in $registry\n";

  # Add to Makefile dependancies
  $depends .= "$file "; 
  # Add to external declarations
  $externs .= "extern codelet $call\;\n"; 
  # Add to registry table
  $structs .= "  { \"$name\", 
    (codelet_apply_fp) &$call }, \n"; 
}

# Output the C registry array (see codelets.c)
open(FD, ">$registry");
print FD <<TEXT
#include "wht.h"
#include "codelets.h"

$externs

static const codelet_apply_entry
codelet_apply_registry[] =
{
$structs
CODELET_APPLY_ENTRY_END
};
TEXT
;
close(FD);

# Output the automake dependancies (see Makefile.am)
open(FD, ">$path/.codelets");
print FD "CODELETS = $depends\n";
close(FD);

# vim: set ts=4 tw=79:

