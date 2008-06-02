#!/usr/bin/env perl -w

use Helpers;
use List::Util qw/max/;

sub splitil {
  return "splitil[" . join(',',@_) . "]";
}

sub smallv {
  my $n = pop @_;
  my $a = join ',', @_;
  return "smallv($a)[$n]";
}


my $n = int($ENV{'MAX_UNROLL'});
my $k = int($ENV{'MAX_INTERLEAVE'});
my $v = int($ENV{'VECTOR_SIZE'});
my $p = log2($v) + 1; # Min size for vectorization by v
my $r = max(log2($k), log2(2*$v)); # Min size for max interleave


print "\nVectorization Tests\n\n";

for (my $i = $p; $i <= $n; $i++) {
  expect_correct    smallv($v,1,$i);
  expect_correct    smallv($v,0,$i);
  expect_reject     splitd(smallv($v,1,$i), smallv($v,1,$i));
  expect_reject     splitd(smallv($v,0,$i), smallv($v,0,$i));
  expect_correct    splitd(smalld($p), smallv($v,1,$i));
  expect_correct    splitd(smalld($p), smallv($v,0,$i));
  expect_reject     splitd(smalld($p), smallv($v,1,$i), smalld(1));
  expect_reject     splitd(smalld($p), smallv($v,0,$i), smalld(1));
  expect_correct    splitd(splitil(smalld($p),smallv($v,0,$i)), smallv($v,1,$p))
}

# Vary interleave factor fix size
for (my $i = $p; $i <= $n; $i++) {
  expect_correct  splitil(smallv($v,$v,1,$p), smallv($v,1, $i));
  expect_correct  splitil(smallv($v,$v,0,$p), smallv($v,1, $i));

  expect_correct  splitil(smallv($v,$v,1,1), 
    splitil(smallv($v,$v,1,1),smallv($v,1,$i)));

  expect_correct  splitil(smallv($v,$v,0,1), 
    splitil(smallv($v,$v,0,1),smallv($v,1,$i)));

  # Reject aligned vectors here
  expect_reject   splitil(
    splitil(smallv($v,$v,1,1), smallv($v,0,$p)),
    splitil(smallv($v,$v,1,1), smallv($v,1,$i)));

  # Accept general vectors here
  expect_correct  splitil(
    splitil(smallv($v,$v,0,1), smallv($v,0,$p)),
    splitil(smallv($v,$v,0,1), smallv($v,1,$i)));

  # Reject when interleaved codelets do not have splitil parent
  expect_reject  splitd(smallv($v,$v,1,$p), smallv($v,1, $i));
  expect_reject  splitd(smallv($v,$v,0,$p), smallv($v,1, $i));

  # General vectorized form
  expect_correct  splitil(smallv($v,$k,0,$i),smallv($v,1,$r));
}

# Reject when interleaved codelets do not have parent
for (my $x = 2; $x <= $k; $x*=2) {
  expect_reject   smallv($v,$x,0,$n);
  expect_reject   smallv($v,$x,1,$n);
}

