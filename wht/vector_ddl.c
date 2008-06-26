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
 * \file vector_ddl.c
 * \author Michael Andrews
 *
 * \brief Implementation of vectorization breakdown rules.
 */

#include "wht.h"
#include "codelets.h"
#include "registry.h"

void 
split_vector_ddl_apply(Wht *W, long S, size_t D, wht_value *x)
{
  Wht *Wi;
  long N, Ni, j, R, T;
  size_t v;

  v   = W->attr[VECTOR_SIZE];

  N   = W->N;

  Wi  = W->children->Ws[0];
  Ni  = W->children->ns[0];
  R   = 1;
  T   = v;

  (Wi->apply)(Wi, S*v, S, x);

  Wi  = W->children->Ws[1];
  Ni  = W->children->ns[1];
  R   = N / v;
  T   = 1;

  for (j = 0; j < R; j++)
    (Wi->apply)(Wi, S, S, x+j*Ni);

}

/**
 * \brief Replace split node with vector split node if possible.
 *
 * \param   W   WHT Plan
 */
void
split_vector_ddl_rule(Wht *W)
{
  Wht *Wl, *Wr;

  /* Check that codelet is split */
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split for ddl");

  if (W->children->nn != 2)
    return error_msg_set(W, "codelet must be binary split for ddl");

  W->attr[VECTOR_SIZE] = W->rule->params[0];

#if 0
  if (! i_power_of_2(v))
    return error_msg_set(W, "vector length must be a power of 2");

  W->attr[VECTOR_SIZE] = v;
#endif

#if 0
  Wr = W->children->Ws[0]; /* Rightmost in Binary split */
  Wl = W->children->Ws[1]; /* Leftmost in Binary split */

  if (Wr->children == NULL) {

  } else {

  }
#endif


  W->apply = split_vector_ddl_apply;
}

#include "simd.h"

void apply_small1_v2(Wht *W, long S, long U, wht_value *x)
{
#if (2 == WHT_VECTOR_SIZE)
  wht_vector2 ta1;
  wht_vector2 ta2;
  wht_vector2 ta3;
  wht_vector2 ta4;
  wht_vector2 ta5;

  vload2(ta1,x[0]);

  vshuf2(ta2,ta1,ta1,0,1);

  vadd2(ta3,ta1,ta2);
  vsub2(ta4,ta2,ta1);

  vshuf2(ta5,ta3,ta4,1,1);

  vstore2(ta5,x[0]);

#else
  wht_exit("initialization guards should prevent this message");
#endif
}

void
small_vector_ddl_rule(Wht *W)
{
  W->attr[VECTOR_SIZE] = W->rule->params[0];

  switch (W->attr[VECTOR_SIZE]) {
    case 2:
      if (W->N != 2)
        return error_msg_set(W, "codelet must be size 2^1 for v = 2");

      W->apply = &apply_small1_v2;
      break;
    case 4:
      if (W->N != 4)
        return error_msg_set(W, "codelet must be size 2^2 for v = 4");

      // W->apply = &apply_small2_v4;
      break;
    default:
      return error_msg_set(W, "not codelets for vector size");
      break;
  }
}

