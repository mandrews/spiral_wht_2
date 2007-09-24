/*
 * Copyright (c) 2000 Carnegie Mellon University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "randtree.h"
#include "rand.h"
#include "wht.h"
#include <stdio.h>
#include <stdlib.h>

extern int min_leaf, max_leaf;

int 
num_1bits(long vect)
{
  int num = 0;
  while (vect != 0) {
    if ((vect & 1) == 1)
      num++;
    vect = vect >> 1;
  }
  return num;
}

/*
 * Sean Eron Anderson
 * seander@cs.stanford.edu
From: http://graphics.stanford.edu/~seander/bithacks.html

"
Brian Kernighan's method goes through as many iterations as there are set bits. 
...

Published in 1988, the C Programming Language 2nd Ed. 
(by Brian W. Kernighan and Dennis M. Ritchie) mentions this 
in exercise 2-9. On April 19, 2006 Don Knuth pointed out to me that 
this method "was first published by Peter Wegner in CACM 3 (1960), 322. 
(Also discovered independently by Derrick Lehmer and published in 1964 
in a book edited by Beckenbach.)"

unsigned int v; // count the number of bits set in v
unsigned int c; // c accumulates the total bits set in v
for (c = 0; v; c++)
{
    v &= v - 1; // clear the least significant bit set
}

*/

node *
rec_uniform_splitting(int size)
{
  node *tree;
  long splitting;
  int num_children;
  int child_size;
  int child_index;

  if (size > (sizeof(int) * 8)) 
    wht_error("2^%d too large for register shifting operations", size);

  do {
    splitting = rand_int(1 << (size - 1));
    splitting |= 1 << (size - 1);
    num_children = num_1bits(splitting);
  } while (num_children == 1 && size > max_leaf);

  if (num_children == 1) {
    tree = node_init(size, 0);
  } else {
    tree = node_init(size, num_children);
    child_size = 1;
    child_index = 0;
    while (splitting != 0) {
      if ((splitting & 1) == 1) {
        tree->children[child_index] = rec_uniform_splitting(child_size);
        tree->children[child_index]->parent = tree;
        tree->children[child_index]->parent_index = child_index;
        child_index++;
        child_size = 1;
      } else {
	      child_size++;
      }
      splitting = splitting >> 1;
    }
  }

  return tree;

}

node *
rec_series_splitting(int size)
{
  node *tree;
  int num_children;
  int children_sum;
  int children[MAX_SPLIT_NODES];
  int i;

  /* Sanity check */
  if (size < min_leaf) 
    wht_error("node is smaller than minimum leaf");

  /* If we are within twice the min leaf size, make it a leaf */
  if (size < 2 * min_leaf) {
    return node_init(size, 0);
  }

  /* If we are at a possible leaf size,
     give a 50-50 chance of making it a leaf */
  if (size <= max_leaf && rand_int(2) >= 1) {
    return node_init(size, 0);
  }

  children[0] = rand_int(size - 2 * min_leaf + 1) + min_leaf;
  num_children = 1;
  children_sum = children[0];
  while (children_sum < size) {
    if (size - children_sum < 2 * min_leaf) {
      children[num_children] = size - children_sum;
    } else {
      children[num_children] =
	    rand_int(size - children_sum - 2 * min_leaf + 1) + min_leaf;
    }
    children_sum += children[num_children];
    num_children++;
  }

  tree = node_init(size, num_children);
  for (i = 0; i < num_children; i++) {
    tree->children[i] = rec_series_splitting(children[i]);
    tree->children[i]->parent = tree;
    tree->children[i]->parent_index = i;
  }

  return tree;
}


