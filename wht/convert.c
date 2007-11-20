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


#include "wht.h"

#include "getopt.h"

static void
usage() 
{
  printf("Usage: wht_convert -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w PLAN   Verify correctness of PLAN.\n");
  printf("    -i FACTOR Interleave PLAN by FACTOR.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *wht_plan;
  int interleave;
  int vectorize;
  int c;

  wht_plan = NULL;
  interleave = -1;
  vectorize  = -1;

  opterr = 0;

  while ((c = getopt (argc, argv, "hw:i:v:")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
        break;
      case 'i':
        interleave = abs(atoi(optarg));
        break;
      case 'v':
        vectorize = abs(atoi(optarg));
        break;
      case 'h':
        usage();
#if 0
      case 'v':
        wht_info();
        exit(0);
#endif
      default:
        usage();
    }

  if (wht_plan == NULL)
    usage();

  Wht *W;

  W = wht_parse(wht_plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  char *buf;

#if 0
  if (vectorize > 0 && interleave > 0)
    Wp = vector_convert(W, vectorize, interleave);
  else if (vectorize > 0)
    Wp = vector_convert(W, vectorize, vectorize);
  else if (interleave > 0)
    Wp = interleave_convert(W, interleave);
  else
    Wp = wht_parse(wht_to_string(W));
#endif

  if (interleave > 0) {
    int params[] = { interleave };
    interleave_convert(W, params, 1);
  } 

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  buf = wht_to_string(W);
  printf("%s\n", buf);
  free(buf);

  wht_free(W);

  return 0;
}
