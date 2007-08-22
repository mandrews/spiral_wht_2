#!/usr/bin/env perl
#  default.pl
# 
#  short description
#
#  Author: Michael Andrews <mjand@drexel.edu>
#  Version: 
#  Date:
#

$small    = 8;
$vector   = 2;
$whtgen   = "../../whtgen/whtgen";
$registry = "codelet_registry.h";

$codelets = "";
$externs  = "";
$files    = "";
$size     = 0;

for ($i=1;$i<=$small;++$i) {
   $files .= "s_$i.c ";
  `$whtgen -n $i > s_$i.c`;
   $codelets .= 
<<TEXT
  { "apply_small$i",
    &apply_small$i
  },
TEXT
;
  $size++;
;

  $externs .= "extern codelet apply_small$i\;\n";
}

$v = (log($vector) / log(2)) + 1;

for ($i=$v;$i<=$small;++$i) {
   $files .= "s_$i\_v$vector.c ";
  `$whtgen -n $i -v $vector > s_$i\_v$vector.c`;
   $codelets .= 
<<TEXT
  { "apply_small$i\_v$vector",
    &apply_small$i\_v$vector
  },
TEXT
;
  $externs .= "extern codelet apply_small$i\_v$vector\;\n";
  $size++;
}

open(FD, ">$registry");
print FD <<TEXT
#include "wht.h"

$externs

static const int wht_codelet_registry_size = $size;

static const codelet_entry
wht_codelet_registry[] =
{
$codelets
};
TEXT
;
close(FD);

open(FD, ">.codelets");
print FD "CODELETS = $files\n";
close(FD);

# vim: set ts=4 tw=79:

