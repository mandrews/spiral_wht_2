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
 * \file classify.c
 * \author Michael Andrews
 *
 * \brief Interface to classify aspects of a WHT plan.
 */

#define _GNU_SOURCE

#include "wht.h"

#include <getopt.h>
#include <string.h>
#include <stdio.h>

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
plan_width(Wht *W)
{
  int i;
  unsigned int nn, mn;

  if (W->children == NULL)
    return 1;

  nn = W->children->nn;
  mn = nn;

  for (i = 0; i < nn; i++) 
    mn = max(mn, plan_width(W->children->Ws[i]));

  return mn;
}

#if 0
unsigned int
plan_last_size(Wht *W)
{
  Wht *Wi;
  unsigned int rs;

  Wi = W;
  while (Wi->children != NULL) {
    Wi = Wi->children->Ws[0];
  }

  rs = Wi->n;

  return rs;
}

unsigned int
plan_first_size(Wht *W)
{
  size_t nn;
  unsigned int fs;

  if (W->children == NULL) 
    return W->n;

  nn = W->children->nn;
  fs = W->children->Ws[nn - 1]->n;

  return fs;
}

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

static void
usage() 
{
  printf("Usage: wht_classify -w PLAN [OPTIONS]\n");
  printf("Classify PLAN from stdin or by argument.\n");
  printf("    -h            Show this help message.\n");
  printf("    -v            Show build information.\n");
  printf("    -w PLAN       Classify WHT PLAN.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan;
  size_t len;
  int c;

  plan = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:")) != -1)
    switch (c) {
      case 'w':
        plan = strdup(optarg);
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (plan == NULL)
    getline(&plan, &len, stdin);

  if (plan == NULL)
    usage();

  Wht *W;

  W = wht_parse(plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  printf("depth:  %u\n", plan_depth(W));
  printf("extent: %u\n", plan_extent(W));
  printf("width:  %u\n", plan_width(W));
#if 0
  unsigned int n = plan_extent(W);

  printf("interleave:     %f\n", plan_attrs(W,interleave_by) / ((n-1) * 8.0));
  printf("vectorize:      %f\n", plan_attrs(W,vector_size) / (n * 2.0));
#endif


  if (plan_is_balanced(W))
    printf("shape:  balanced\n");
  if (plan_is_rightmost(W))
    printf("shape:  right\n");
  if (plan_is_leftmost(W))
    printf("shape:  left\n");
  if (plan_is_iterative(W))
    printf("shape:  iterative\n");

  wht_free(W);
  free(plan);

  return 0;
}

#ifndef DOXYGEN_MAN_MODE
/**
\page wht_classify Classify a WHT Plan

\section _synopsis SYNOPSIS
wht_classify -w PLAN

\section _description DESCRIPTION

Classify a WHT plan based on a series of metrics.

\verbatim
Classify PLAN from stdin or by argument.
    -h            Show this help message.
    -v            Show build information.
    -w PLAN       Classify WHT PLAN.
\endverbatim

\section _examples EXAMPLES

A balanced WHT plan

\verbatim
wht_classify -w 'split[split[small[1],small[2]],split[small[1],small[4]]]'
depth:  3
extent: 4
width:  2
shape:  balanced
\endverbatim

A leftmost WHT plan

\verbatim
wht_classify -w 'split[split[small[1],small[1]],small[1]]'
depth:  3
extent: 3
width:  2
shape:  left
\endverbatim

A rightmost WHT plan

\verbatim
wht_classify -w 'split[small[1],split[small[1],small[1]]]'
depth:  3
extent: 3
width:  2
shape:  right
\endverbatim

*/
#endif/*DOXYGEN_MAN_MODE*/
