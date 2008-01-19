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

/**
 * \file wht/split.c
 *
 * \brief Implementation of methods for applying WHT recursive split
 */

#include "wht.h"

/**
 *  A \f$ {\bf WHT}_{2^n} \f$ can be split into \f$ k \f$ \f$ {\bf WHT} \f$ 's of smaller size
 *  (according to \f$ n = n_1 + n_2 + \cdots + n_t \f$):
 *
  \f$ 
   \prod_{i=1}^t ({\bf I}_{2^{n_1 + \cdots + n_{i-1}}} 
      \otimes {\bf WHT}_{2^{n_i}}
      \otimes {\bf I}_{2^{n_{i+1} + \cdots + n_t}}) 
  \f$
 *
 * The \f$ {\bf WHT}_{2^n} \f$ is performed by stepping through this product
 * from right to left.
 */
void 
split_apply(Wht *W, long S, size_t U, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;

  nn = W->children->nn;

  N  = W->N;
  R  = N;
  S1 = 1;


  /* Iterate over children WHTs, stored anti lexigraphically  */
  for (i = 0; i < nn; i++) {
    Ni = W->children->ns[i];
    R /= Ni;

    for (j = 0; j < R; j++)
      for (k = 0; k < S1; k++)
        (W->children->Ws[i]->apply)(W->children->Ws[i], S1*S, S, x+k*S+j*Ni*S1*S);

    S1 *= Ni;
  }
}

Wht *
split_init(Wht *Ws[], size_t nn)
{
  Wht *W;
  size_t i;
  long N = 1;
  int n  = 0;
  size_t right, left; 

  /* Compute size of WHT from smaller WHTs*/
  for (i = 0; i < nn; i++) {
    N *= Ws[i]->N;
    n += Ws[i]->n;
  }

  W           = node_init(n, "split");
  W->apply    = split_apply;

  W->children = i_malloc(sizeof(split_children));

  W->children->nn = nn;

  left  = W->left;
  right = W->right;

  /* Store smaller WHTs */
  for (i = 0; i < nn; i++) {
    W->children->Ws[i] = Ws[i];
    W->children->ns[i] = Ws[i]->N;

    W->children->Ws[i]->parent = W;
    W->children->Ws[i]->left   = left;
    W->children->Ws[i]->right  = right;

    right *= Ws[i]->N;
    left  /= Ws[i]->N;
  }

  return W;
}

