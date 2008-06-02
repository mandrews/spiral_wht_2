#!/usr/bin/env perl -w

use Helpers;

print "\nClassify Tests\n\n";

expect_classify   

  smalld(1), 

  { 'left_tree'   => 0,
    'right_node'  => 1 };

expect_classify   

  splitd(
    smalld(1), smalld(1)), 

  { 'left_tree'   => 0,
    'right_node'  => 1 };

expect_classify   

  splitd(
    splitd(
      smalld(1),smalld(1)),
    smalld(1)), 

  { 'left_tree'   => 2,
    'right_node'  => 1 };

expect_classify   

  splitd(
    splitd(
      smalld(4),smalld(2)),
    smalld(1)), 

  { 'left_tree'   => 6,
    'right_node'  => 1 };

expect_classify
  splitd(
    smalld(2), splitd(
      smalld(1), smalld(2))),

   { 'left_tree'  => 0,
     'right_node' => 2 };

expect_classify   

  splitd(
    splitd(
      smalld(4),smalld(2)),
    splitd(
      smalld(1),smalld(2))), 
      
   { 'left_tree'  => 6,
     'right_node' => 2 };

expect_classify   

  splitd(
    splitd(
      splitd(
        smalld(4),smalld(2)),
      splitd(
        smalld(3),smalld(2))),
    splitd(
      smalld(1),smalld(2))), 
      
   { 'left_tree'  => 11,
     'right_node' =>  2 };

