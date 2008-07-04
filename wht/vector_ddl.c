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

void split_vector_apply(Wht *W, long S, size_t U, wht_value *x);

void 
split_vector_apply(Wht *W, long S, size_t D, wht_value *x)
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

  if (Wi->requires[VECTOR_STRIDE]) {
    (Wi->apply)(Wi, S*v, S, x);
  } else {
    for (j = 0; j < S*v; j++)
      (Wi->apply)(Wi, S*v, S, x + j*S);
  }

  Wi  = W->children->Ws[1];
  Ni  = W->children->ns[1];
  R   = N / (v*v);
  T   = 1;

  if (Wi->requires[VECTOR_SQUARE_STRIDE]) {
    for (j = 0; j < R; j++)
      (Wi->apply)(Wi, S*v, S, x+j*Ni*v);
  } else {
    for (j = 0; j < R*v; j++)
      (Wi->apply)(Wi, S, S, x+j*Ni);
  }
}

/**
 * \brief Replace split node with vector split node if possible.
 *
 * \param   W   WHT Plan
 */
void
split_vector_rule(Wht *W)
{
  Wht *Wl, *Wr;
  size_t v;

  v = W->rule->params[0];

  /* Check that codelet is split */
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split for ddl");

  if (W->children->nn != 2)
    return error_msg_set(W, "codelet must be binary split for ddl");

  W->attr[VECTOR_SIZE] = v;

  W->provides[VECTOR_STRIDE] = true;

  if (! i_power_of_2(v))
    return error_msg_set(W, "vector length must be a power of 2");

  Wr = W->children->Ws[0]; /* Rightmost in Binary split */
  Wl = W->children->Ws[1]; /* Leftmost in Binary split */

  if (Wr->N < Wl->N)
    return error_msg_set(W, "Rightmost tree must be larger than v");

  if (Wl->requires[VECTOR_STRIDE])
    return error_msg_set(W, "Leftmost tree cannot be applied at stride v");

  if (Wl->N != v)
    return error_msg_set(W, "Leftmost tree must have size v");

  if (parent_provides(W, VECTOR_STRIDE)) 
    return error_msg_set(W, "parent codelet cannot change stride");

  W->apply = split_vector_apply;
}

void
small_vector_rule_3(Wht *W)
{
  size_t v;

  v = W->rule->params[0];

  if ((W->parent == NULL) || (! W->parent->provides[VECTOR_STRIDE]))  
    return error_msg_set(W, "parent codelet must apply at stride v");

  if (W->N != v)
    return error_msg_set(W, "codelet must be size v");

  W->attr[VECTOR_SIZE] = v;
  W->requires[VECTOR_SQUARE_STRIDE] = true;

  W->apply = codelet_apply_lookup(W);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");
}

