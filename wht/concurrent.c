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
 * \file concurrent.c
 * \author Michael Andrews
 *
 * \brief Concurrent breakdown rules implementations.
 *
 */

#include "wht.h"

#ifdef WHT_HAVE_OMP
#include <omp.h>
#endif/*WHT_HAVE_OMP*/

#define OMP_CHUNK   (256)

void split_concurrent_apply(Wht *W, long S, size_t U, wht_value *x);

void
split_concurrent_rule(Wht *W)
{
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be interleaved");

  W->apply = split_concurrent_apply;
}

void 
split_concurrent_apply(Wht *W, long S, size_t U, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;
  Wht *Wi;

  nn = W->children->nn;

  N  = W->N;
  R  = N;
  S1 = 1;

  /* Iterate over children WHTs, stored anti lexigraphically  */
  for (i = 0; i < nn; i++) {
    Ni = W->children->ns[i];
    Wi = W->children->Ws[i];
    R /= Ni;

    #pragma omp parallel for private(k) schedule(guided, OMP_CHUNK)
    for (j = 0; j < R; j++) 
      for (k = 0; k < S1; k++) 
        (Wi->apply)(Wi, S1*S, S, x+k*S+j*Ni*S1*S);

    S1 *= Ni;
  }
}

