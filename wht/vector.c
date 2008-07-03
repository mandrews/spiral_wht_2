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
 * \file vector.c
 * \author Michael Andrews
 *
 * \brief Implementation of vectorization breakdown rules.
 *
 * Vectorization provides a way of computing base cases of the form:
 *
 * \WHT_2v (Rightmost)
 *
 * and
 *
 * \WHT_2n_X_I_2v  (General)
 *
 * Where v is the vector length.
 *
 * Vectorization has the potential to decrease the number of loads, stores and
 * arithmetic operation by a factor of v.  Vectorization can also be coupled
 * with interleaving to potentially reduce data cache misses.
 */

#include "wht.h"
#include "codelets.h"
#include "registry.h"

/**
 * \brief Check to see if a WHT plan is rightmost.
 *
 * \param   W   WHT plan
 * \return      True if plan is rightmost, false otherwise
 */
bool
rightmost_tree(Wht *W)
{
  Wht *Wp;
  size_t right;

  Wp = W->parent;
  right = 1;

  while (Wp != NULL) {
    right *= Wp->right;
    Wp = Wp->parent;
  }

  return (right == 1);
}

/**
 * \brief Replace small node with rightmost vectorized node if possible.
 *
 * \param   W   WHT Plan
 *
 * The node within the plan must meet the following criteria for the %rule to
 * attach:
 */
void
small_vector_rule_2(Wht *W)
{
  size_t v, a;

  v = W->rule->params[0];
  a = W->rule->params[1];

  /**  The package must be configured for vectors of size v */
  if (v != WHT_VECTOR_SIZE)
    return error_msg_set(W, "not configured for vectors of size %d",v);

  /**  
   * If thie node loads aligned memory (e.g. at unit stride) the node must
   * be in a rightmost plan
   */
  if ((a == 1) && ! rightmost_tree(W))
    return error_msg_set(W, "must be in rightmost tree of plan");

  /**  The node must be the rightmost node in the plan */
  if (W->right != 1)
      return error_msg_set(W, "must be rightmost codelet in plan");

  /**  The node must be have at least size 2v */
  if (2*v > W->N)
    return error_msg_set(W, "size of codelet must be at least 2v = %d", 2*v);

  W->apply = codelet_apply_lookup(W);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");

  W->attr[VECTOR_SIZE] = v;
}

/**
 * \brief Replace small node with a vectorized small node if possible.
 
 * \param   W   WHT Plan
 *
 * The node within the plan must meet the following criteria for the %rule to
 * attach:
 */
void
small_vector_rule_1(Wht *W)
{
  size_t v, k, a;

  v = W->rule->params[0];
  k = W->rule->params[1];
  a = W->rule->params[2];

  /**  The package must be configured for vectors of size v */
  if (v != WHT_VECTOR_SIZE)
    return error_msg_set(W, "not configured for vectors of size %d",v);

  /**  
   * If thie node loads aligned memory (e.g. at unit stride) the node must
   * be in a rightmost plan
   */
  if (! parent_provides(W,VECTOR_STRIDE))
    if ((a == 1) && (! rightmost_tree(W)))
      return error_msg_set(W, "must be in rightmost tree of plan");

  /**  The node must be right of at least one node in plan */
  if (! parent_provides(W,VECTOR_STRIDE))
    if (W->right == 1)
      return error_msg_set(W, "cannot be rightmost codelet in plan");

  if (! parent_provides(W, VECTOR_STRIDE)) {
    W->rule->params[0] = k;
    W->rule->params[1] = UNSET_PARAMETER;
    W->rule->params[2] = UNSET_PARAMETER;
    W->rule->n = 1;
    strcpy(W->rule->name, "smallil");

    /**  
     * All rules regarding the interleaving of this node must be satisfied 
     * \see small_interleave_rule
     */

    small_interleave_rule(W);
  }

  W->rule->params[0] = v;
  W->rule->params[1] = k;
  W->rule->params[2] = a;
  W->rule->n = 3;
  strcpy(W->rule->name, "smallv");

  W->apply = codelet_apply_lookup(W);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");

  W->attr[VECTOR_SIZE] = v;
  W->attr[INTERLEAVE_BY] = k;

  if (parent_provides(W, VECTOR_STRIDE)) {
    W->requires[VECTOR_STRIDE] = true;
    set_parent_requires(W, VECTOR_STRIDE);
  }

  /** 
   * \todo Check and test (size * interleave_by) > VECTOR_SIZE.
   * This only occurs with vectors of size >= 4  
   */

  /** 
   * \todo Figure out an elegant way of reusing rules
   */

}

