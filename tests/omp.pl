#!/usr/bin/env perl -w

use Helpers;

sub splitp {
  return "splitp[" . join(',',@_) . "]";
}

sub splitpddl {
  my $b = shift;
  return "splitddlp($b)[" . join(',',@_) . "]";
}

sub smallil {
  my $k = shift @_;
  my $n = shift @_;
  return "smallil($k)[$n]";
}

$ENV{'OMP_NUM_THREADS'} ||= 2; # Set number of openMP threads if not set

my $n = int($ENV{'MAX_UNROLL'});
my $k = int($ENV{'MAX_INTERLEAVE'});
my $b = 256;

printf "\nopenMP Tests (%d threads)\n\n", $ENV{'OMP_NUM_THREADS'};

for (my $i = 1; $i <= $n; $i++) {
  expect_correct  splitp(smalld(1), smalld($i));

  expect_correct  splitp(
    splitd(smalld(1), smalld(1)),
    splitd(smalld(1), smalld($i)));

  expect_correct  splitp(
    splitp(smalld(1), smalld(1)),
    splitp(smalld(1), smalld($i)));
}

for (my $i = 1; $i <= $n; $i++) {
  expect_reject   splitp(smalld(1), smalld(1), smalld($i));
}

for (my $i = 1; $i <= $n; $i++) {
  for (my $j = 4; $j < $b; $j*=2) {
    expect_correct splitpddl($j, smalld(1), smalld($i));

    expect_correct splitpddl($j, smalld($i), smalld(1));

    expect_correct splitpddl($j, 
      splitd(smalld(1), smalld(1)),  
      splitd(smalld(1), smalld($i)) );

    expect_correct splitpddl($j, 
      splitp(smalld(1), smalld(1)),  
      splitp(smalld(1), smalld($i)) );
  }
}

expect_reject  splitpddl(0, smalld(1), smalld(1));
expect_reject  splitpddl(1, smalld(1), smalld(1));
expect_reject  splitpddl(2, smalld(1), smalld(1));
expect_reject  splitpddl(3, smalld(1), smalld(1));
expect_reject  splitpddl(5, smalld(1), smalld(1));

expect_reject  splitpddl(4, smalld(1), smalld(1), smalld(1));

if ($k > 2) {
  my $K = log2($k); # Minimum codelet size to perform interleaving up to $k

  for (my $i = 2; $i <= $k; $i *= 2) {
    expect_correct  splitp(smallil($i,1),smalld($K));
  }
}

