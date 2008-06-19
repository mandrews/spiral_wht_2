package Helpers;

require(Exporter);

our @ISA = qw(Exporter);
our @EXPORT = qw/ splitd smalld 
                  expect_correct 
                  expect_reject 
                  expect_classify
                  expect_rotate 
                  log2 /;
our @EXPORT_OK = qw/ path verify classify rotate /;

use strict;

use File::Basename qw/ basename dirname /;
use POSIX qw/ ceil floor /;

our $path       = dirname($0);

our $verify     = "$path/../wht/wht_verify";
our $classify   = "$path/../wht/wht_classify";

our $rotate     = "$path/../rand/wht_rotate";

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

sub expect_classify {
  my $plan = shift @_;
  my $hash = shift @_;
  my $out  = `$classify -w '$plan'`;

  for my $k ( keys %$hash ) {
    if ($out =~ /$k\s*:\s*(.*)/) {
      my $v = $1;

      if ($hash->{$k} == $v) {
        print "PASS[A] $plan\n";
      } else {
        print "FAIL[R] $plan\n";
        print "!!!\n";
        exit 1;
      }
    } else {
      print "FAIL[E] NO '$k' IN '$classify'\n";
      print "!!!\n";
      exit 1;
    }
  }
}

sub expect_rotate {
  my $p   = shift @_;
  my $old = shift @_;
  my $new = shift @_;
  my $out = `$rotate -w '$old' -p $p`;

  chomp($out);

  if ($out eq $new) {
    print "PASS[A] $old\n";
  } else {
    print "FAIL[R] $old\n";
    print "!!!\n";
    exit 1;
  }
}

1;

