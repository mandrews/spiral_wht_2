#!/usr/bin/env perl
#  register_codelets.pl
#
#  Registers codelets in the codelets_registry and with the automake
#  system.
#
#  Author: Michael Andrews <mjand@drexel.edu>
#  Version: 
#  Date:

use File::Basename qw/ basename dirname /;

$path           = dirname($0);
$registry       = "$path/codelet_registry.h";

@files = <$path/*.c>;

$externs  = "";
$structs  = "";
$depends  = "";

foreach $file (@files) {
  {
    local $/;
    open(FD, $file) or die "Could not open $file: $!.";
    $data = <FD>; # Read in entire file as a sting
    close(FD) or die "Could not close $file: $!.";
  }

  if ($data =~ /\s*EXTERNS\s*:\s*{\s*(.*?)\s*}\s*/s) {
    $externs .= $1;
    if (not $depends =~ basename($file)) {
      $depends .= basename($file);
      $depends .= ' ';
    }
  }

  if ($data =~ /\s*STRUCTS\s*:\s*{(.*?)\s*}\s*/s) {
    $structs .= $1;
    if (not $depends =~ basename($file)) {
      $depends .= basename($file);
      $depends .= ' ';
    }
  }
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

