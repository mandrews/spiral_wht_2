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

bool
plan_is_iterative(Wht *W)
{
  int i;

  if (W->children == NULL)
    return true;
  else
    for (i = 0; i < W->children->nn; i++)
      if (W->children->Ws[i]->children != NULL)
        return false;

  return true;
}

bool
plan_is_recursive(Wht *W)
{
  return (! plan_is_iterative(W));
}

bool
plan_is_rightmost(Wht *W)
{
  int i, j;
  bool accept;

  if (W->children == NULL)
    return true;

  accept = true;

  for (i = W->children->nn - 1; (i >= 0) && (W->children->Ws[i]->children == NULL); i--);

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

bool
plan_is_balanced(Wht *W)
{
  size_t nn;
  int i;
  Wht *Wi;
  bool accept;

  if (W->children == NULL)
    return true;

  accept = true;

  nn = W->children->nn;
  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    if (Wi->children !=NULL && Wi->children->nn != nn)
      return false;
    else
      accept = accept && plan_is_balanced(Wi);
  }

  return accept;
}

unsigned int
plan_arity(Wht *W)
{
  int i;
  unsigned int nn, ni, max;

  if (W->children == NULL)
    return 1;

  nn  = W->children->nn;
  max = nn;

  for (i = 0; i < nn; i++) {
    ni = plan_arity(W->children->Ws[i]);
    if (ni > max)
      max = ni;
  }

  return max;
}

static void
usage() 
{
  printf("Usage: wht_classify -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w PLAN   Classify Wht PLAN.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *wht_plan;
  int c;

  wht_plan = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
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
    usage();

  Wht *W;
  char *buf;

  W = wht_parse(wht_plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  buf = W->to_string(W);

#if 0
  printf("%s\n", buf);
#endif

  if (plan_is_iterative(W))
    printf("iterative ");
  else
    printf("recursive ");

  if (plan_is_balanced(W))
    printf("balanced ");
  else if (plan_is_rightmost(W))
    printf("right ");
  else if (plan_is_leftmost(W))
    printf("left ");
  else
    printf("mixed ");

  printf("%d-arity ", plan_arity(W));

  printf("\n");

  wht_free(W);
  free(buf);

  return 0;
}
