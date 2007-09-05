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
wht_apply_split(Wht *W, long S, long D, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;
  long nIL;

  nn = W->priv.split.nn;

  N  = W->N;
  R  = N;
  S1 = 1;

  /* step through the smaller whts */
  for (i = 0; i < nn; i++) {
    Ni = W->priv.split.ns[i];
    R /= Ni;

    nIL = (W->priv.split.Ws[i])->nILNumber;
    for (j = 0; j < R; j++)
      for (k = 0; k < S1; k+=nIL)
         wht_apply(W->priv.split.Ws[i], S1*S, S, x+k*S+j*Ni*S1*S);

    S1 *= Ni;
  }
}

static void 
wht_free_split(Wht *W) 
{
  int i;

  for (i = 0; i < W->priv.split.nn; i++) {
    W->priv.split.Ws[i]->free(W->priv.split.Ws[i]);
  }
  wht_free(W);
}

Wht *
wht_init_split(Wht *Ws[], size_t nn) 
{
  Wht *W;
  long i;
  long N = 1;
  int n  = 0;

  /* compute size of wht */
  for (i = 0; i < nn; i++) {
    N *= Ws[i]->N;
    n += Ws[i]->n;
  }

  W            = wht_init_codelet(n);
  W->apply     = wht_apply_split;
  W->free      = wht_free_split;
  W->priv.split.nn = nn;

  /* store smaller whts */
  for (i = 0; i < nn; i++) {
    W->priv.split.Ws[i] = Ws[i];
    W->priv.split.ns[i] = Ws[i]->N;
  }

  return W;
}

