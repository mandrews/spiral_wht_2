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
 * \file parallel.c
 * \author Kang Chen
 *
 * \brief Concurrent breakdown rules implementations.
 *
 */

#include "wht.h"
#include "registry.h"

#ifdef WHT_HAVE_OMP
#include <omp.h>
#endif/*WHT_HAVE_OMP*/

void split_parallel_apply(Wht *W, long S, size_t U, wht_value *x);

void
split_parallel_rule(Wht *W)
{
  size_t b;

  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be parallelized");

  if (W->children->nn != 2)
    return error_msg_set(W, "codelet must be a binary split to be parallelized");

  W->apply = split_parallel_apply;
}

void 
split_parallel_apply(Wht *W, long S, size_t U, wht_value *x) 
{
#ifdef WHT_HAVE_OMP
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
#else
  wht_exit("initialization guards should prevent this message");
#endif
}

/** \todo import splitddl_p */
