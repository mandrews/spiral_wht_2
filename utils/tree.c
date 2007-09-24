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

#include "tree.h"
#include "wht.h"

node *
node_init(int size, int num_children)
{
  int i;
  node *np;

  np = wht_malloc(sizeof(*np));

  np->size = size;
  np->parent = NULL;
  np->parent_index = -1;
  np->num_children = num_children;

  if (num_children == 0) {
    np->children = NULL;
  } else {
    np->children = wht_malloc(num_children * sizeof(*np->children));
    for (i = 0; i < num_children; i++) {
      np->children[i] = NULL;
    }
  }

  return np;
}


void 
node_free(node *node)
{
  int i;

  for (i = 0; i < node->num_children; i++) 
    node_free(node->children[i]);

  wht_free(node);
}

Wht *
node_to_wht(node *n)
{
  size_t i;

  small f;
  split g;

  Wht *Ws[MAX_SPLIT_NODES];

  if (n->num_children == 0) {
    f = lookup_small("small", 0);
    return (f)("small", n->size, NULL, 0);
  } else {
    for (i =0; i < n->num_children; i++)
      Ws[i] = node_to_wht(n->children[i]);

    g = lookup_split("split", 0);
    return (g)("split", Ws, i, NULL, 0);
  }
}

char* 
tree_to_formula(node* tree)
{
  Wht *W;
  char *buf;

  W = node_to_wht(tree);

  buf = W->to_string(W);
  W->free(W);

  return buf;
}
