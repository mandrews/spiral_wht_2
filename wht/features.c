/*
 * Copyright (c) 2007 Drexel University
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


/**
 * \file features.c
 * \author Michael Andrews
 *
 * \brief Functions to measure features of a WHT tree.
 */

#include "wht.h"

#define min(a,b) ((a < b) ? (a) : (b))
#define max(a,b) ((a > b) ? (a) : (b))

unsigned int
plan_extent(Wht *W)
{
  int i;
  unsigned d;

  if (W->children == NULL)
    return 1;

  d = 0;
  for (i = 0; i < W->children->nn; i++)
    d += plan_extent(W->children->Ws[i]);

  return d;
}

unsigned int
plan_depth(Wht *W)
{
  int i;
  unsigned d;

  if (W->children == NULL)
    return 1;

  d = 1;
  for (i = 0; i < W->children->nn; i++)
    d = max(d, 1 + plan_depth(W->children->Ws[i]));

  return d;
}

unsigned int
plan_min_width(Wht *W)
{
  int i;
  unsigned int nn, mn;

  if (W->children == NULL)
    return 1;

  nn = W->children->nn;
  mn = nn;

  for (i = 0; i < nn; i++) 
    mn = min(mn, plan_min_width(W->children->Ws[i]));

  return mn;
}

unsigned int
plan_max_width(Wht *W)
{
  int i;
  unsigned int nn, mn;

  if (W->children == NULL)
    return 1;

  nn = W->children->nn;
  mn = nn;

  for (i = 0; i < nn; i++) 
    mn = max(mn, plan_max_width(W->children->Ws[i]));

  return mn;
}

unsigned int
plan_min_size(Wht *W)
{
  int i;
  unsigned int nn, mn;

  if (W->children == NULL)
    return W->n;

  nn = W->children->nn;
  mn = W->n;

  for (i = 0; i < nn; i++) 
    mn = min(mn, plan_min_size(W->children->Ws[i]));

  return mn;
}

unsigned int
plan_max_size(Wht *W)
{
  int i;
  unsigned int nn, mn;

  if (W->children == NULL)
    return W->n;

  nn = W->children->nn;
  mn = 0;

  for (i = 0; i < nn; i++) 
    mn = max(mn, plan_max_size(W->children->Ws[i]));

  return mn;
}

#if 0
int
plan_attrs(Wht *W, size_t attr)
{
  int i, d;

  d = max(W->attr[attr], 0);
  if (W->children == NULL)
    return d;

  for (i = 0; i < W->children->nn; i++)
    d += plan_attrs(W->children->Ws[i], attr);

  return d;
}
#endif

#if 0
/* Fuzzy rightmost and leftmost */
bool
plan_is_rightmost(Wht *W)
{
  int i, j;
  bool accept;

  if (W->children == NULL)
    return true;

  accept = true;

  /* Walk from right to left and stop when there is a leaf node */
  for (i = W->children->nn - 1; (i >= 0) && (W->children->Ws[i]->children == NULL); i--);

  /* The tree is not rightmost if there is a leaf to left of this node */
  for (j = i; j >= 0; j--) 
    if (W->children->Ws[j]->children == NULL) 
      return false;
    else
      accept = accept && plan_is_rightmost(W->children->Ws[j]);

  return accept;
}

bool
plan_is_leftmost(Wht *W)
{
  int i, j;
  bool accept;

  if (W->children == NULL)
    return true;

  accept = true;

  for (i = 0; (i < W->children->nn) && (W->children->Ws[i]->children == NULL); i++);

  for (j = i; j < W->children->nn; j++) 
    if (W->children->Ws[j]->children == NULL)
      return false;
    else
      accept = accept && plan_is_leftmost(W->children->Ws[j]);

  return accept;
}
#endif

bool
plan_is_iterative(Wht *W)
{
  int i;
  size_t nn;

  if (W->children == NULL || W->children->nn == 0)
    return true;

  nn = W->children->nn;
  for (i = 0; i < nn; i++)
    if (W->children->Ws[i]->children != NULL)
      return false;

  return true;
}

bool
plan_is_rightmost(Wht *W)
{
  int i;
  size_t nn;

  if (plan_is_iterative(W))
    return true;

  nn = W->children->nn;
  for (i = 1; i < nn; i++)
    if (W->children->Ws[i]->children != NULL)
      return false;

  return plan_is_rightmost(W->children->Ws[0]);
}

bool
plan_is_leftmost(Wht *W)
{
  int i;
  size_t nn;

  if (plan_is_iterative(W))
    return true;

  nn = W->children->nn;
  for (i = 0; i < nn - 1; i++)
    if (W->children->Ws[i]->children != NULL)
      return false;

  return plan_is_leftmost(W->children->Ws[nn - 1]);
}

bool
plan_is_balanced(Wht *W)
{
  size_t nn, ns;
  int i;
  Wht *Wi;
  bool accept;

  if (plan_is_iterative(W))
    return true;

  Wi = NULL;
  nn = W->children->nn;
  ns = 0;

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    if (Wi->children != NULL)
      ns += Wi->children->nn;
  }

  if (ns / nn != nn)
    return false;

  accept = true;

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    if (Wi->children != NULL)
      accept = accept && plan_is_balanced(Wi);
  }
  
  return accept;
}


/**
 * \todo Move the rightmost_tree code from vector.c to feature.c
 */
bool rightmost_tree(Wht *W);

unsigned int
plan_left_size(Wht *W)
{
  int i;
  unsigned int left;

  if (W->children == NULL) {
    if (! rightmost_tree(W) && W->right != 1)
      return W->n;
    else
      return 0;
  }

  left = 0;
  for (i = 0; i < W->children->nn; i++)
    left += plan_left_size(W->children->Ws[i]);

  return left;
}

#undef min
#undef max

