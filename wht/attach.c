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
  printf("Usage: wht_attach -w PLAN -r RULE [OPTIONS]\n");
  printf("Attach a RULE to WHT PLAN from stdin or by argument.\n");
  printf("    -h            Show this help message.\n");
  printf("    -v            Show build information.\n");
  printf("    -w PLAN       WHT PLAN.\n");
  printf("    -r RULE       Attach RULE to PLAN.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan, *rule;
  size_t len;
  int c;

  plan  = NULL;
  rule  = NULL;
  len   = 0;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:r:")) != -1)
    switch (c) {
      case 'w':
        plan = strdup(optarg);
        break;
      case 'r':
        rule = strdup(optarg);
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

  if (rule == NULL)
    usage();

  Wht *W;

  W = wht_parse(plan);

  rule_eval_from_string(W, rule);

  if (wht_error_msg(W) != NULL) {
    fprintf(stderr, "rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  printf("%s\n", W->to_string);

  free(plan);
  free(rule);

  wht_free(W);

  return 0;
}

#ifndef DOXYGEN_MAN_MODE
/**
\page wht_attach Attach a Breakdown Rule to a WHT Plan

\section _synopsis SYNOPSIS
Usage: wht_attach -w PLAN -r RULE [OPTIONS]

\section _description DESCRIPTION

Attaches and evaluates a %rule in the context of a WHT plan.  If the rule cannot be bound to 
a node in the plan, that node is reverted to it's previous state.

\verbatim
Attach a RULE to WHT PLAN from stdin or by argument.
    -h            Show this help message.
    -v            Show build information.
    -w PLAN       WHT PLAN.
    -r RULE       Attach RULE to PLAN.
\endverbatim

\section _examples EXAMPLES

Note the syntax for split rule must be a string in the grammar.

\verbatim
echo 'split[small[1],small[4]]' | wht_attach -r 'splitil[small[0]]'
splitil[small[1],small[4]]

echo 'split[small[1],small[4]]' | wht_attach -r 'splitil[small[0]]' | wht_attach -r 'smallil(2)[0]'
splitil[smallil(2)[1],small[4]]
\endverbatim

Also the order of evaling rules is important.  For instance this plan 
cannot be interleaved by 4, since it does not have a split interleave rule applied 
to as it's parent.

\verbatim
echo 'split[small[1],small[4]]' | wht_attach -r 'smallil(2)[0]' | wht_attach -r 'splitil[small[0]]'
splitil[small[1],small[4]]
\endverbatim

Much of these complications are hidden in the wht_interleave shell script, and other rule 
application scripts.

*/
#endif/*DOXYGEN_MAN_MODE*/
