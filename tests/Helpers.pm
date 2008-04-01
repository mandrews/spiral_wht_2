package Helpers;

require(Exporter);

our @ISA = qw(Exporter);
our @EXPORT = qw/splitd smalld expect_correct expect_reject log2/;
our @EXPORT_OK = qw/verify path/;

use strict;

use File::Basename qw/ basename dirname /;
use POSIX qw/ceil floor/;

our $path     = dirname($0);
our $verify   = "$path/../wht/wht_verify";

sub splitd {
  return "split[" . join(',',@_) . "]";
}

sub smalld {
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

1;

