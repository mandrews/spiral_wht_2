#!/usr/bin/env perl -w

use Helpers;

my $n = int($ENV{'MAX_UNROLL'});

print "\nDefault Tests\n\n";

for (my $i = 1; $i <= $n; $i++) {
  expect_correct smalld($i);
  expect_correct splitd(smalld($i), smalld(1));
}

for (my $i = $n+1; $i <= 2*$n; $i++) {
  expect_reject smalld($i);
  expect_reject splitd(smalld($i), smalld(1));
}

