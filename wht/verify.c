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
 * \file verify.c
 *
 * \brief Interface to verify the correctness of a WHT plan.
 */

#define _GNU_SOURCE

#include "wht.h"

#include <getopt.h>
#include <string.h>
#include <stdio.h>

static void
usage() 
{
  printf("Usage: wht_verify -w PLAN [OPTIONS]\n");
  printf("Verify PLAN from stdin or by argument.\n");
  printf("    -h            Show this help message.\n");
  printf("    -v            Show build information.\n");
  printf("    -w PLAN       Verify correctness of PLAN.\n");
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
  Wht *D;
  wht_value *x, *y;

  long n,N;
  int i;

  W = wht_parse(plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    free(plan);
    wht_free(W);
    exit(1);
  }

  n = W->n;
  N = W->N;

  D = wht_direct(n);

  x = wht_random_vector(N);
  y = malloc(sizeof(wht_value) * N);

  for (i = 0;i<N;i++)
    y[i] = x[i];

  wht_apply(W,x);
  wht_apply(D,y);

  /* printf("%s\n", W->to_string); */
  printf("%g\n", wht_max_norm(x,y,N)); 

  if (wht_max_norm(x,y,N) < WHT_STABILITY_THRESHOLD)
    printf("correct\n");   
  else
    printf("incorrect\n");

  wht_free(W);
  wht_free(D);

  free(x);
  free(y);

  free(plan);

  return 0;
}

#ifndef DOXYGEN_MAN_MODE
/**
\page wht_verify Verify a WHT Plan

\section _synopsis SYNOPSIS
wht_verify -w PLAN

\section _description DESCRIPTION

Empirically verify that a plan calculates the WHT transform by comparing 
the result with direct computation.

\verbatim
Verify PLAN from stdin or by argument.
    -h            Show this help message.
    -v            Show build information.
    -w PLAN       Verify correctness of PLAN.
\endverbatim

\section _examples EXAMPLES

Verify the correctness of a plan
\verbatim
wht_verify -w 'split[small[2],small[2]]'
correct
\endverbatim

If a codelet in a plan has not been registered properly, or does not exist
the plan is rejected
\verbatim
wht_verify -w 'split[small[10],small[2]]'
rejected, not configured for unrolled codelets of size 10 @ small[10]
\endverbatim

If a codelet has a semantic error, i.e. it does not accurately compute the WHT
the plan is incorrect
\verbatim
wht_verify -w 'split[new_small[4],small[2]]'
incorrect
\endverbatim

*/
#endif/*DOXYGEN_MAN_MODE*/
