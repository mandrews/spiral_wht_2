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
  long N, Ni, j, k, R, T;
  size_t v;

  v   = W->attr[VECTOR_SIZE];

  N   = W->N;

  Wi  = W->children->Ws[0];
  Ni  = W->children->ns[0];
  R   = N / v;
  T   = 1;

  for (j = 0; j < R; j++)
    (Wi->apply)(Wi, S, S, x+j*Ni);

  Wi  = W->children->Ws[1];
  Ni  = W->children->ns[1];
  R   = 1;
  T   = v;

  (Wi->apply)(Wi, S*v, S, x);
}

/**
 * \brief Replace split node with vector split node if possible.
 *
 * \param   W   WHT Plan
 */
void
split_vector_ddl_rule(Wht *W)
{
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

  /* W->provides[INTERLEAVING] = true; */

  W->apply = split_vector_ddl_apply;
}

void
small_vector_ddl_rule(Wht *W)
{
  W->apply = codelet_apply_lookup(W);

  W->attr[VECTOR_SIZE] = W->rule->params[0];

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");
}

