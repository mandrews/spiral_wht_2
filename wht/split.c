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
wht_apply_split(Wht *W, long S, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;

  nn = W->priv.split.nn;

  N  = W->N;
  R  = N;
  S1 = 1;


  /* step through the smaller whts */
  for (i = 0; i < nn; i++) {
    Ni = W->priv.split.ns[i];
    R /= Ni;

    size_t kp = W->priv.split.Ws[i]->attr[interleave_by];

    for (j = 0; j < R; j++)
      for (k = 0; k < S1; k+=kp)
         wht_apply(W->priv.split.Ws[i], S1*S, x+k*S+j*Ni*S1*S);

    S1 *= Ni;
  }
}

static void 
wht_free_split(Wht *W) 
{
  int i;

  for (i = 0; i < W->priv.split.nn; i++) 
    W->priv.split.Ws[i]->free(W->priv.split.Ws[i]);

  wht_free(W);
}

char *
split_to_string(Wht *W)
{
  const size_t bufsize = 8; /*split[ ... ]\0*/

  char *buf, *tmp;
  size_t nn, i, j, len, resize;

  buf = wht_malloc(sizeof(char) * bufsize);

  snprintf(buf, bufsize - 1, "split[");

  nn = W->priv.split.nn;

  resize = bufsize;

  /* Iterate over children whts, stored anti lexigraphically  */
  for (i = 0; i < nn; i++) {
    j       = nn - i - 1;
    tmp     = W->priv.split.Ws[j]->to_string(W->priv.split.Ws[j]);
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
wht_init_split(Wht *Ws[], size_t nn) 
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

  W            = wht_init_codelet(n);
  W->apply     = wht_apply_split;
  W->free      = wht_free_split;
  W->to_string = split_to_string;


  W->priv.split.nn = nn;

  /* store smaller whts */
  for (i = 0; i < nn; i++) {
    right *= Ws[i]->N;
    Ws[i]->guard(Ws[i], right);
    W->priv.split.Ws[i] = Ws[i];
    W->priv.split.ns[i] = Ws[i]->N;
  }

  return W;
}

