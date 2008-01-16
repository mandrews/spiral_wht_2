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
  printf("Usage: wht_convert -w PLAN -t TRANSFORM [OPTIONS]\n\n");
  printf("    -h            Show this help message.\n");
  printf("    -v            Show build information.\n");
  printf("    -w PLAN       Verify correctness of PLAN.\n");
  printf("    -t TRANSFORM  Transform plan with.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan, *func;
  size_t len;
  int c;

  plan  = NULL;
  func  = NULL;
  len   = 0;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:t:")) != -1)
    switch (c) {
      case 'w':
        plan = strdup(optarg);
        break;
      case 't':
        func = strdup(optarg);
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

  if (func == NULL)
    usage();

  Wht *W;
  char *buf;

  W = wht_parse(plan);

  transform_from_string(W, func);

  if (wht_error_msg(W) != NULL) {
    fprintf(stderr, "rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  buf = wht_to_string(W);
  printf("%s\n", buf);

  free(buf);
  free(plan);
  free(func);

  wht_free(W);

  return 0;
}
