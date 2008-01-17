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

#define _GNU_SOURCE

#include "wht.h"

#include <getopt.h>
#include <string.h>
#include <stdio.h>

static void
usage() 
{
  printf("Usage: wht_verify -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w PLAN   Verify correctness of PLAN.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *wht_plan;
  size_t len;
  int c;

  wht_plan = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:")) != -1)
    switch (c) {
      case 'w':
        wht_plan = strdup(optarg);
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (wht_plan == NULL)
    getline(&wht_plan, &len, stdin);

  if (wht_plan == NULL)
    usage();

  Wht *W;
  Wht *D;
  wht_value *x, *y;
  char *buf;

  long n,N;
  int i;

  W = wht_parse(wht_plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
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

  buf = W->to_string(W);

  if (wht_max_norm(x,y,N) < WHT_STABILITY_THRESHOLD)
    printf("correct\n");   
  else
    printf("accepted but incorrect\n");

  wht_free(W);
  wht_free(D);

  free(x);
  free(y);

  free(buf);
  free(wht_plan);

  return 0;
}


/**
\page man_wht_verify wht_verify
 
\section _usage Usage
\verbatim
Usage: wht_verify -w PLAN [OPTIONS]
    -h        Show this help message.
    -v        Show build information.
    -w PLAN   Verify correctness of PLAN.
\endverbatim

\section _examples Examples

-# Verify correctness of small vectorized codelet
\code
$ wht_verify -w 'smallv(2)[2]'
correct

$ echo 'splitil[smallv(2,2,1)[2],smallv(2)[2]]' | wht_verify 
correct
\endcode

*/
