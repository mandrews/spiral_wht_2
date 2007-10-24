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
        interleave = atoi(optarg);
        break;
      case 'v':
        vectorize = atoi(optarg);
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

  Wht *W, *Wp;

  W = wht_parse(wht_plan);

  if (wht_accept(W) == false) {
    wht_free(W);
    printf("%s\n", wht_error_msg);
    exit(1);
  }

  if (vectorize > 0 && interleave > 0)
    Wp = vector_convert(W, vectorize, interleave);
  else if (vectorize > 0)
    Wp = vector_convert(W, vectorize, vectorize);
  else if (interleave > 0)
    Wp = interleave_convert(W, interleave);
  else
    Wp = wht_parse(wht_to_string(W));

  if (wht_accept(Wp) == false) {
    wht_free(W);
    wht_free(Wp);
    printf("%s\n", wht_error_msg);
    exit(1);
  }

  printf("%s\n", wht_to_string(Wp));

  wht_free(W);
  wht_free(Wp);

  return 0;
}
