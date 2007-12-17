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
  unsigned int n;

  W = wht_parse(wht_plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  n = plan_extent(W);

  printf("depth:          %u\n", plan_depth(W));
  printf("extent:         %u\n", n);
  printf("width:          %u\n", plan_width(W));
  printf("first:          %u\n", plan_first_size(W));
  printf("last:           %u\n", plan_last_size(W));
  printf("interleave:     %f\n", plan_attrs(W,interleave_by) / ((n-1) * 8.0));
  printf("vectorize:      %f\n", plan_attrs(W,vector_size) / (n * 2.0));


  printf("shape:          ");
  if (plan_is_rightmost(W))
    printf("right");
  else if (plan_is_leftmost(W))
    printf("left");
  else if (plan_is_balanced(W))
    printf("balanced");
  else
    printf("mixed");
  printf("\n");

  wht_free(W);

  return 0;
}
