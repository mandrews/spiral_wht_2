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
 * \file interleave.c
 * \author Michael Andrews
 *
 * \brief Implementation of loop interleaving breakdown rules.
 *
 * Loop interleaving provides a way of computing base cases of the form:
 *
 * \WHT_2n_X_I_k
 *
 * Instead of looping over \I_k, the computations of \WHT_2n are 
 * interleaved, i.e., computed concurrently. 
 * This has the potential to decrease data cache misses.
 */

#include "wht.h"
#include "codelets.h"
#include "registry.h"

void split_interleave_apply(Wht *W, long S, size_t U, wht_value *x);

/**
 * \brief Replace small node with interleaved small node if possible.
 *
 * \param   W   WHT Plan
 */
void
small_interleave_rule(Wht *W)
{
  size_t k, k_max;

  k = W->rule->params[0];

  k_max = WHT_MAX_INTERLEAVE;

  /* Check that codelet is small */
  if (W->children != NULL)
    return error_msg_set(W, "codelet must be small to be interleaved");

  /* Check that parent codelet is split interleaved */
#if 0
  /* an attribute like, provides_interleaving within root node should work */
  if ((W->parent == NULL) || (strcmp("splitil", W->parent->rule->name) != 0)) 
    return error_msg_set(W, "codelet must be used in conjunction with splitil");
#endif

  /* Check that interleave factor is supported */
  if (k > k_max) 
    return error_msg_set(W, 
      "not configured for codelets of size %zd interleaved by %zd", W->n, k);

  /* Check it is possible to interleave */
  if (W->right <= 1) 
    return error_msg_set(W, "cannot be interleaved");

  /* Check that interleave factor is less than right I_n */
  if (k > W->right) 
    return error_msg_set(W, "interleave factor must be <= %d", W->right);

  W->apply = codelet_apply_lookup(W);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");

  W->attr[interleave_by] = k;
}

/**
 * \brief Replace split node with interleaved split node if possible.
 *
 * \param   W   WHT Plan
 */
void
split_interleave_rule(Wht *W)
{
  /* Check that codelet is split */
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be interleaved");

  W->apply = split_interleave_apply;
}


/**
 * \brief Apply transform using interleaed codelets.
 *
 * Only significant difference between this implementation and \ref split_apply
 * is step inside the innermost for loop.
 *
 * \param   W   WHT plan
 * \param   S   Stride to apply transform at
 * \param   U   Base stride
 * \param   x   Input vector
 */
void 
split_interleave_apply(Wht *W, long S, size_t U, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;
  size_t nk;
  Wht *Wi;

  nn = W->children->nn;

  N  = W->N;
  R  = N;
  S1 = 1;

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    Ni = W->children->ns[i];
    R /= Ni;

    if (Wi->attr[interleave_by] != UNSET_ATTRIBUTE)
      nk = Wi->attr[interleave_by];
    else
      nk = 1;

    for (j = 0; j < R; j++)
      for (k = 0; k < S1; k+=nk)
        (Wi->apply)(Wi, S1*S, S, x+k*S+j*Ni*S1*S);

    S1 *= Ni;
  }
}

