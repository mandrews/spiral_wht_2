#!/usr/bin/env perl -w

use Helpers;

print "\nClassify Tests\n\n";

expect_classify   

  smalld(1), 

  'left_size', 0;

expect_classify   

  splitd(
    smalld(1), smalld(1)), 

  'left_size', 0;

expect_classify   

  splitd(
    splitd(
      smalld(1),smalld(1)),
    smalld(1)), 

  'left_size', 1;

expect_classify   

  splitd(
    splitd(
      smalld(4),smalld(2)),
    smalld(1)), 

  'left_size', 4;

expect_classify
  splitd(
    smalld(2), splitd(
      smalld(1), smalld(2))),

   'left_size', 0;

expect_classify   

  splitd(
    splitd(
      smalld(4),smalld(2)),
    splitd(
      smalld(1),smalld(2))), 
      
  'left_size', 4;

expect_classify   

  splitd(
    splitd(
      splitd(
        smalld(4),smalld(2)),
      splitd(
        smalld(3),smalld(2))),
    splitd(
      smalld(1),smalld(2))), 
      
  'left_size', 7;

