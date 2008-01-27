#!/usr/bin/env perl

#
# Copyright (c) 2007 Drexel University
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#


#
#  File: register_codelets.pl
#  Author: Michael Andrews 
#
#  Registers codelets in the codelets_registry and with the automake
#  system.
#

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

