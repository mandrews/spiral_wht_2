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
  size_t b;

  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be interleaved");

  b = (1 << W->rule->params[0]);

  W->attr[omp_chunk] = b;
  W->apply = split_concurrent_apply;
}

void 
split_concurrent_apply(Wht *W, long S, size_t U, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;
  Wht *Wi;
  int b;
  
  b = W->attr[omp_chunk];

  nn = W->children->nn;

  N  = W->N;
  R  = N;
  S1 = 1;

  /* Iterate over children WHTs, stored anti lexigraphically  */
  #pragma omp parallel for ordered schedule(guided, b) \
    shared(i,Ni,Wi,R,S1) private(j,k) 
  for (i = 0; i < nn; i++) {
    #pragma omp ordered
    {
      Ni = W->children->ns[i];
      Wi = W->children->Ws[i];
      R /= Ni;
    }

    for (j = 0; j < R; j++) 
      for (k = 0; k < S1; k++) 
        (Wi->apply)(Wi, S1*S, S, x+k*S+j*Ni*S1*S);

    #pragma omp ordered 
    {
      S1 *= Ni;
    }
  }
}

#if 0

/* Kang's scheduled code from previous version of package.  Early testing seems
 * to indicate that that openMP scheduling does a better job.
 */

void 
split_kang_apply(Wht *W, long S, size_t U, wht_value *x) 
{
  long block, chunk;
  long N, R, S1, Ni, j;
  wht_value *xpt;
  long total, id;

  if (omp_in_parallel()) {
    N  = W->N;
    Ni = W->children->ns[1];
    R = W->children->ns[0];

    block = Ni * S;
    for (j = 0, xpt = x; j < R; ++ j, xpt += block)
      (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);

    for (j = 0, xpt = x; j < Ni; ++ j, xpt += S)
      (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
  } 

  else {
#pragma omp parallel private (xpt, id, j, chunk) 
    { /* start of parallel region */
      N  = W->N;
      Ni = W->children->ns[1];
      R = W->children->ns[0];
      total = omp_get_num_threads();
      block = Ni * S;  

      /* no scheduling
      id = omp_get_thread_num();
      while (id < R) {
        xpt = x + id * block;  
        (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
        id += total;
      }*/

      chunk = R / total;
      if (chunk * total == R) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * block;
          (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= R - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * block;
            (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
          }
        } else if (id < R) {
          for (; id < R; ++ id) {
            xpt = x + id * block;
            (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
          }
        }
      }

#pragma omp barrier
      chunk = Ni / total;
      if (chunk * total == Ni) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * S;
          (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= Ni - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * S;
            (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
          }
        } else if (id < Ni) {
          for (; id < Ni; ++ id) {
            xpt = x + id * S;
            (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
          }
        }
      }
    } /*end of parallel region*/
  }
}
#endif
