/*
 * Copyright (c) 2007 Drexel University
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
 * \file transpose.c
 * \author Michael Andrews
 *
 * \brief Matrix transposition routines, acts on input vector
 *
 */

#include "wht.h"

void
transpose(wht_value *x, size_t m, size_t n)
{
  size_t i, j;
  wht_value *y;

  y = i_malloc(m*n * sizeof(*x));

  for (i = 0; i < m*n; i++)
    y[i] = x[i]; 

  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      x[j*m + i] = y[i*n + j];
    }
  }

  free(y);
}

bool
transpose_accept(Wht *W)
{
  return (W->children->nn == 2) && split_accept(W);
}

void 
transpose_apply(Wht *W, long S, size_t D, wht_value *x)
{
  size_t N, N1, N2, j;
  wht_value *xs;

  N  = W->N;
  N1 = W->children->ns[0];
  N2 = W->children->ns[1];

  /* Compute right transform first */
  for (j = 0, xs = x; j < N1; j++, xs += (N2 * S))
    (W->children->Ws[1]->apply)(W->children->Ws[1], S, 0, xs);

  /* L_N^N2 */
  transpose(x, N1, N2);

  /* Compute transposed left transform */
  for (j = 0, xs = x; j < N2; j++, xs += (N1 * S))
    (W->children->Ws[0]->apply)(W->children->Ws[0], S, 0, xs);

  /* L_N^N1 */
  transpose(x, N2, N1);
}

Wht *
transpose_init(char *name, Wht *Ws[], size_t nn, int params[], size_t np) 
{
  Wht *W;

  W            = split_init(name, Ws, nn, params, np);
  W->apply     = transpose_apply;
  W->accept    = transpose_accept;

  return W;  
}

