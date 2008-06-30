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
 * \file rotate.cxx
 * \author Michael Andrews
 */
extern "C" {
#include "wht.h"
}

#include "combin.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

static void
usage() 
{
  printf("Usage: wht_rotate -w PLAN [OPTIONS]\n");
  printf("    -h            Show this help message.\n");
  printf("    -v            Show build information.\n");
  printf("    -w PLAN       Rotate PLAN.\n");
  printf("    -p MIN_E      Minimum element in composition.\n");
  printf("    -q MAX_E      Maximum element in composition.\n");
  printf("    -u UNSTRICT   Generate WHT trees that cannot be executed in current configuration.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan;
  size_t min_leaf, max_leaf, len;
  bool strict;

  int c;

  opterr = 0;

  plan      = NULL;
  min_leaf  = 1;
  max_leaf  = WHT_MAX_UNROLL;
  strict    = true;

  while ((c = getopt (argc, argv, "hvw:p:q:u")) != -1)
    switch (c) {
      case 'w':
        plan = strdup(optarg);
        break;
      case 'p':
        min_leaf = atoi(optarg);
        break;
      case 'q':
        max_leaf = atoi(optarg);
        break;
      case 'u':
        strict = false;
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

  if (min_leaf < 1) {
    printf("-a MIN_E too small.\n");
    exit(1);
  }

  if ((min_leaf > WHT_MAX_UNROLL) && strict) {
    printf("-a MIN_E cannot be larger than %d.\n", WHT_MAX_UNROLL);
    exit(1);
  }

  if ((max_leaf > WHT_MAX_UNROLL) && strict) {
    printf("-b MAX_E cannot be larger than %d.\n", WHT_MAX_UNROLL);
    exit(1);
  }

  Wht *W1, *W2;
  compos_node *root;


  W1 = NULL;

  W1 = wht_parse(plan);

  if (wht_error_msg(W1) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W1));
    free(plan);
    wht_free(W1);
    exit(1);
  }

  root = wht_to_compos_tree(W1);

  compos_tree_rotate(root, min_leaf, max_leaf);

  /* compos_tree_print(W2); */

  W2 = compos_tree_to_wht(root);

  //assert(W1->n == W2->n);

  printf("%s\n", W2->to_string);

  compos_tree_free(root);

  free(plan);

  wht_free(W1);
  wht_free(W2);

  return 0;
}

