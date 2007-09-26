/*
 * Copyright (c) 2000 Carnegie Mellon University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the wht_free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the wht_free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "wht.h"

/* Split WHT
   ---------
   A WHT_N can be split into k WHT's of smaller size
   (according to N = N_1 * N_2 * ... * N_k):

                              WHT_N_1 tensor 1_(N/N_1) *
     1_N_1             tensor WHT_N_2 tensor 1_(N/N_1N_2) *
       ...
       ...
     1_(N_1...N_(k-1)) tensor WHT_N_k 

   The WHT_N is performed by stepping through this product
   from right to left.
*/

static void 
split_apply(Wht *W, long S, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;

  nn = W->children->nn;

  N  = W->N;
  R  = N;
  S1 = 1;


  /* step through the smaller whts */
  for (i = 0; i < nn; i++) {
    Ni = W->children->ns[i];
    R /= Ni;

    size_t kp = W->children->Ws[i]->attr[interleave_by];

    for (j = 0; j < R; j++)
      for (k = 0; k < S1; k+=kp)
         (W->children->Ws[i]->apply)(W->children->Ws[i], S1*S, x+k*S+j*Ni*S1*S);

    S1 *= Ni;
  }
}

static void 
split_free(Wht *W) 
{
  int i;

  for (i = 0; i < W->children->nn; i++) 
    W->children->Ws[i]->free(W->children->Ws[i]);

  wht_free(W->children);
  wht_free(W);
}

char *
split_to_string(Wht *W)
{
  const size_t bufsize = strlen(W->name) + 3; /*IDENT[ ... ]\0*/

  char *buf, *tmp;
  size_t nn, i, j, len, resize;

  buf = wht_malloc(sizeof(char) * bufsize);

  snprintf(buf, bufsize - 2, W->name);
  strncat(buf,"[",1);

  nn = W->children->nn;

  resize = bufsize;

  /* Iterate over children whts, stored anti lexigraphically  */
  for (i = 0; i < nn; i++) {
    j       = nn - i - 1;
    tmp     = W->children->Ws[j]->to_string(W->children->Ws[j]);
    len     = strlen(tmp) + 1; /* Extra 1 is for comma */

    resize += len;
    buf     = realloc(buf, resize);

    strncat(buf, tmp, len);

    if (i < nn - 1)
      strncat(buf, ",", 1);
  }

  strncat(buf,"]",1);

  return buf;
}

Wht *
split_init(char *name, Wht *Ws[], size_t nn, int params[], size_t pn) 
{
  Wht *W;
  size_t i;
  long N = 1;
  int n  = 0;
  int right = 1;

  /* compute size of wht */
  for (i = 0; i < nn; i++) {
    N *= Ws[i]->N;
    n += Ws[i]->n;
  }

  W            = codelet_init(n, name);
  W->apply     = split_apply;
  W->free      = split_free;
  W->to_string = split_to_string;

  W->children = wht_malloc(sizeof(split_children));

  W->children->nn = nn;

  /* store smaller whts */
  for (i = 0; i < nn; i++) {
    Ws[i]->guard(Ws[i], right);
    W->children->Ws[i] = Ws[i];
    W->children->ns[i] = Ws[i]->N;
    right *= Ws[i]->N;
  }

  return W;
}

