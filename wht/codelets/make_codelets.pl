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

# TODO 
# These parameters should be command line arguments
# set by ./configure --enable-maintainer-mode
$small      = 4;
$vector     = 2;
$interleave = 4;
$whtgen     = "../../whtgen/whtgen";
$registry   = "codelet_registry.h";

@codelets = ();
$codelets = 0;

# Greps through a codelet file for the function call
sub function_name {
  $file = shift @_;
  $name = "";
  open(FD, $file);
  while (<FD>) {
    next unless $_ =~ /\s*void\s*([\w\d]+)\s*\(.*?\)$/;
    $name = $1;
    last;
  }
  close(FD);
  return $name;
}

# Generate unaligned unrolled codelets
for ($i=1;$i<=$small;++$i) {
  push(@codelets,("s_$i.c", "-n $i"));
  $codelets++;
}

# Generate aligned vectorized codelets
for ($k=1;$k<=$vector;$k++) {
  $v = 2**$k;
  for ($i=$vector+1;$i<=$small;++$i) {
    push(@codelets,("s_$i\_v\_$v\_a.c", "-n $i -v $v -a"));
    $codelets++;
  }
}

# Generate unaligned interleaved codelets
for ($i=1;$i<=$small;++$i) {
  for ($j=1;$j<=$interleave;$j++) {
    $k = 2**$j;
    push(@codelets,("s_$i\_il\_$k.c", "-n $i -i $k"));
    $codelets++;
  }
}

# Generate unaligned vectorized interleaved codelets
for ($i=$vector+1;$i<=$small;++$i) {
  for ($j=$vector;$j<=$interleave;$j++) {
    $l = 2**$j;
    for ($k=1;$k<=$vector;$k++) {
      $v = 2**$k;
      push(@codelets,("s_$i\_il\_$l\_v$v.c", "-n $i -i $l -v $v"));
      $codelets++;
    }
  }
}

$externs  = "";
$structs  = "";
$depends  = "";

while (@codelets) {
  $file = shift @codelets;
  $args = shift @codelets;

  print "Generate:  $whtgen $args > $file\n";
  `$whtgen $args > $file`; # Generate codelet
  $name = function_name($file); # Grep for codelet function name

  print "Register: $name in $registry\n";
  $depends .= "$file "; # Add to dependancies
  $externs .= "extern codelet $name\;\n"; # Add to external declarations
  $structs .= "  { \"$name\",\n  (codelet) &$name\n  },\n"; # Add to registry struct
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

