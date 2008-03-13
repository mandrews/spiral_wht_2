#!/usr/bin/env perl -w

# TODO 
#   - set parameters via enviroment 
#   - collect helper functions into a single non test file

use strict;

use File::Basename qw/ basename dirname /;
use POSIX qw/ceil floor/;
use Getopt::Std;

use vars qw/ %opt /;

our $path     = dirname($0);
our $verify   = "$path/../wht/wht_verify";

sub usage {
  print "omp.pl -n SIZE -k INTERLEAVE_BY\n";
  exit 1;
}

sub _split {
  return "split[" . join(',',@_) . "]";
}

sub _small {
  my $n = shift @_;
  return "small[$n]";
}

sub expect_correct {
  my $plan = shift @_;
  my $out  = `$verify -w '$plan'`;

  if ($out =~ /^correct/) {
    print "PASS[A] $plan\n";
  } else {
    print "FAIL[R] $plan\n";
    print "!!!\n";
    exit 1;
  }
}

sub expect_reject {
  my $plan = shift @_;
  my $out  = `$verify -w '$plan'`;

  if ($out =~ /^reject/) {
    print "PASS[R] $plan\n";
  } else {
    print "FAIL[A] $plan\n";
    print "!!!\n";
    exit 1;
  }
}

sub log2 {
  my $n = shift;
  return ceil(log($n)/log(2));
}

sub _splitp {
  return "splitp[" . join(',',@_) . "]";
}

sub _splitpddl {
  my $b = shift;
  return "splitddlp($b)[" . join(',',@_) . "]";
}

sub _smallil {
  my $k = shift @_;
  my $n = shift @_;
  return "smallil($k)[$n]";
}


getopts('n:k:', \%opt ) or usage;
$opt{'n'} or usage;
$opt{'k'} or usage;

my $n = int($opt{'n'});
my $k = int($opt{'k'});

my $b = 256;

my $i;
my $j;

for ($i = 1; $i <= $n; $i++) {
  expect_correct  _splitp(_small(1), _small($i));
  expect_correct  _splitp(
    _split(_small(1), _small($i)),
    _split(_small(1), _small($i)));
  expect_correct  _splitp(_small($i), _small(1));
}

for ($i = 1; $i <= $n; $i++) {
  expect_reject   _splitp(_small(1), _small(1), _small($i));
}

for ($i=1; $i <= $n;$i++) {
  expect_correct _splitp(_small(1), _splitp(_small(1), _small($i)));
}

my $K = log2($k); # Minimum codelet size to perform interleaving up to $k

for ($i = 2; $i <= $k; $i *= 2) {
  expect_correct  _splitp(_smallil($i,1),_small($K));
}

for ($i = 1; $i <= $n; $i++) {
  for ($j = 4; $j < $b; $j*=2) {
    expect_correct _splitpddl($j, _small(1), _small($i));
    expect_correct _splitpddl($j, _small($i), _small(1));
    expect_correct _splitpddl($j, 
      _split(_small($i), _small(1)),  
      _split(_small($i), _small(1)) );
  }
}

expect_reject  _splitpddl(0, _small(1), _small(1));
expect_reject  _splitpddl(1, _small(1), _small(1));
expect_reject  _splitpddl(2, _small(1), _small(1));
expect_reject  _splitpddl(3, _small(1), _small(1));
expect_reject  _splitpddl(5, _small(1), _small(1));

expect_reject  _splitpddl(4, _small(1), _small(1), _small(1));

