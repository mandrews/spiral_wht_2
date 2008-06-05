#!/usr/bin/env perl -w

use Helpers;

print "\nClassify Tests\n\n";

expect_classify   

  smalld(1), 

  { 'left_tree'   => 0,
    'right_node'  => 2 };

expect_classify   

  splitd(
    smalld(1), smalld(1)), 

  { 'left_tree'   => 0,
    'right_node'  => 4 };

expect_classify   

  splitd(
    splitd(
      smalld(1),smalld(1)),
    smalld(1)), 

  { 'left_tree'   => 16,
    'right_node'  =>  8 };

expect_classify   

  splitd(
    splitd(
      smalld(4),smalld(2)),
    smalld(1)), 

  { 'left_tree'   => 256,
    'right_node'  => 128 };

expect_classify
  splitd(
    smalld(2), splitd(
      smalld(1), smalld(2))),

   { 'left_tree'  =>  0,
     'right_node' => 32 };

expect_classify   

  splitd(
    splitd(
      smalld(4),smalld(2)),
    splitd(
      smalld(1),smalld(2))), 
      
   { 'left_tree'  => 1024,
     'right_node' =>  512 };

expect_classify   

  splitd(
    splitd(
      splitd(
        smalld(4),smalld(2)),
      splitd(
        smalld(3),smalld(2))),
    splitd(
      smalld(1),smalld(2))), 
      
   { 'left_tree'  => 65536 ,
     'right_node' => 16384 };

