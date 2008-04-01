#!/usr/bin/env perl -w

use Helpers;

sub splitil {
  return "splitil[" . join(',',@_) . "]";
}

sub smallil {
  my $k = shift @_;
  my $n = shift @_;
  return "smallil($k)[$n]";
}


my $n = int($ENV{'MAX_UNROLL'});
my $k = int($ENV{'MAX_INTERLEAVE'});

print "\nInterleave Tests\n\n";

for (my $i = 2; $i <= $k; $i*=2) {
  expect_correct splitil(smallil($i,1), smalld($n));
  expect_correct splitil(smallil($i,1), smallil($i,1), smalld($n));
}

# Need to use splitil with smallil
for (my $i = 2; $i <= $k; $i*=2) {
  expect_reject splitd(smallil($i,1), smalld($n));
  expect_reject splitd(smallil($i,1), smallil($i,1), smalld($n));
}

for (my $i=2*$k; $i <= 2*($k+1); $i*=2) {
  expect_reject splitil(smallil($i,1), smalld($n));
}

for (my $i=4; $i <= $k; $i*=2) {
  expect_reject splitil(smallil($i,1), smalld(1));
}

for (my $i=2; $i <= $k; $i*=2) {
  expect_reject splitil(smallil($i,$n));
}

