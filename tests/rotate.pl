#!/usr/bin/env perl -w

use Helpers;

print "\nRotate Tests\n\n";

expect_rotate
  1,
  smalld(1), 
  smalld(1);

expect_rotate
  2,
  smalld(1), 
  smalld(1);

expect_rotate
  1,
  splitd(smalld(1),smalld(1)),
  splitd(smalld(1),smalld(1));

expect_rotate
  2,
  splitd(smalld(1),smalld(1)),
  smalld(2);

expect_rotate
  1,
  splitd(smalld(1),smalld(1),smalld(1)),
  splitd(smalld(1),smalld(1),smalld(1));

expect_rotate
  2,
  splitd(smalld(1),smalld(1),smalld(1)),
  splitd(smalld(1),smalld(2));

expect_rotate
  3,
  splitd(smalld(1),smalld(1),smalld(1)),
  smalld(3);

expect_rotate
  1,
  splitd(smalld(1),smalld(1),smalld(1),smalld(1)),
  splitd(smalld(1),smalld(1),smalld(1),smalld(1));

expect_rotate
  2,
  splitd(smalld(1),smalld(1),smalld(1),smalld(1)),
  splitd(smalld(1),smalld(1),smalld(2));

expect_rotate
  3,
  splitd(smalld(1),smalld(1),smalld(1),smalld(1)),
  splitd(smalld(1),smalld(3));

expect_rotate
  4,
  splitd(smalld(1),smalld(1),smalld(1),smalld(1)),
  smalld(4);

expect_rotate
  2,
  splitd(
    splitd(smalld(1),smalld(1)),
    splitd(smalld(1),smalld(1)) ),
  splitd(smalld(2),smalld(2));

expect_rotate
  3,
  splitd(
    splitd(smalld(1),smalld(1)),
    splitd(smalld(1),smalld(1)) ),
  smalld(4);

expect_rotate
  2,
  splitd(
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(1))),
  splitd(
    splitd(smalld(1),smalld(2)), smalld(2));
  
expect_rotate
  3,
  splitd(
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(1))),
  splitd(smalld(2), smalld(3));

expect_rotate
  2,
  splitd(
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(1))),
  splitd(
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(2)),smalld(2));

expect_rotate
  3,
  splitd(
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(1))),
  splitd(
    smalld(3), smalld(2), smalld(3));

expect_rotate
  4,
  splitd(
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(2)),
    splitd(smalld(1),smalld(1))),
  splitd(
    smalld(3), smalld(5));

expect_rotate
  2,
  splitd(
    splitd(
      splitd(smalld(1),smalld(1),smalld(1)),
      splitd(smalld(1),smalld(1),smalld(1))),
    splitd(
      splitd(smalld(1),smalld(1),smalld(1)),
      splitd(smalld(1),smalld(1),smalld(1)))),
  splitd(
    splitd(
      splitd(smalld(1),smalld(2)),
      splitd(smalld(1),smalld(2))),
    splitd(
      splitd(smalld(1),smalld(2)),
      splitd(smalld(1),smalld(2))));

expect_rotate
  3,
  splitd(
    splitd(
      splitd(smalld(1),smalld(1),smalld(1)),
      splitd(smalld(1),smalld(1),smalld(1))),
    splitd(
      splitd(smalld(1),smalld(1),smalld(1)),
      splitd(smalld(1),smalld(1),smalld(1)))),
  splitd(
    splitd(smalld(3),smalld(3)),
    splitd(smalld(3),smalld(3)));

expect_rotate
  4,
  splitd(
    splitd(
      splitd(smalld(1),smalld(1),smalld(1)),
      splitd(smalld(1),smalld(1),smalld(1))),
    splitd(
      splitd(smalld(1),smalld(1),smalld(1)),
      splitd(smalld(1),smalld(1),smalld(1)))),
  splitd(smalld(6),smalld(6));

expect_rotate
  4,
  splitd(
    splitd(smalld(1),smalld(1)),smalld(4)),
  splitd(smalld(2),smalld(4));

expect_rotate
  2,
  splitd(
    splitd(smalld(1),smalld(1)),
    splitd(smalld(1),smalld(1)),
    splitd(smalld(1),smalld(1)),
    smalld(2)),
  splitd(
    smalld(2),
    smalld(2),
    smalld(2),
    smalld(2));

# TODO Fix these cases when rotate fails with -p = 4
# 'split[small[1],small[1],small[2],small[1],small[1],small[1],split[split[small[1],small[1],small[1]],small[3]],small[3]]'
# 'split[small[1],small[1],small[2],small[1],small[1],small[1],split[split[small[1],small[1],small[1]],small[3]],small[3]]'
# 'split[small[1],small[3],small[2],small[2],small[1],split[split[small[1],small[1]],small[2]],small[3]]'
# 'split[small[1],small[1],small[1],small[1],small[1],small[1],small[1],small[1],small[3],split[split[small[1],small[1],small[1]],small[2]],small[2]]'
# 'split[small[2],small[1],small[1],small[1],split[small[1],small[1]],small[1],split[small[2],small[1],split[split[small[1],small[1]],small[2]],split[small[1],small[1]]],small[3]]''
