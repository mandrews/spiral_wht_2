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

  printf("depth:        %u\n", plan_depth(W));
  printf("extent:       %u\n", plan_extent(W));
  printf("min_width:    %u\n", plan_min_width(W));
  printf("max_width:    %u\n", plan_max_width(W));
  printf("min_size:     %u\n", plan_min_size(W));
  printf("max_size:     %u\n", plan_max_size(W));
  printf("left_tree:    %u\n", plan_left_tree(W));
  printf("right_node:   %u\n", plan_right_node(W));
#if 0
  unsigned int n = plan_extent(W);

  printf("interleave:     %f\n", plan_attrs(W,interleave_by) / ((n-1) * 8.0));
  printf("vectorize:      %f\n", plan_attrs(W,vector_size) / (n * 2.0));
#endif


  if (plan_is_iterative(W))
    printf("shape:        iterative\n");
  else if (plan_is_balanced(W))
    printf("shape:        balanced\n");
  else if (plan_is_rightmost(W))
    printf("shape:        right\n");
  else if (plan_is_leftmost(W))
    printf("shape:        left\n");
  else
    printf("shape:        mixed\n");

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
depth:        3
extent:       4
min_width:    1
max_width:    2
min_size:     1
max_size:     4
shape:        balanced
\endverbatim

A leftmost WHT plan

\verbatim
wht_classify -w 'split[split[small[1],small[1]],small[1]]'
depth:        3
extent:       3
min_width:    1
max_width:    2
min_size:     1
max_size:     1
shape:        left
\endverbatim

A rightmost WHT plan

\verbatim
wht_classify -w 'split[small[1],split[small[1],small[1]]]'
depth:        3
extent:       3
min_width:    1
max_width:    2
min_size:     1
max_size:     1
shape:        right
\endverbatim

*/
#endif/*DOXYGEN_MAN_MODE*/
