#include "wht.h"
#include "codelets.h"
#include "registry.h"

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

void
small_right_vector_rule(Wht *W)
{
  size_t v;

  v = W->rule->params[0];

  if (v != WHT_VECTOR_SIZE)
    return error_msg_set(W, "not configured for vectors of size %d",v);

  if (! (rightmost_tree(W) && (W->right == 1)))
    return error_msg_set(W, "must be rightmost codelet in plan");

  W->apply = codelet_apply_lookup(W);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");

  W->attr[vector_size] = v;
}

void
small_vector_rule(Wht *W)
{
  /** 
   * \todo Check assumptions about number of parameters
   */
  size_t v, k, a;

  v = W->rule->params[0];
  k = W->rule->params[1];
  a = W->rule->params[2];

  if (v != WHT_VECTOR_SIZE)
    return error_msg_set(W, "not configured for vectors of size %d",v);

  if ((a == 1) && (! rightmost_tree(W)))
    return error_msg_set(W, "must be in rightmost tree of plan");

  /** 
   * \todo Check and test (size * interleave_by) > vector_size.
   * This only occurs with vectors of size >= 4  
   */

  W->rule->params[0] = k;
  W->rule->params[1] = UNSET_PARAMETER;
  W->rule->params[2] = UNSET_PARAMETER;
  W->rule->n = 1;
  strcpy(W->rule->ident,"smallil");

  small_interleave_rule(W);

  W->rule->params[0] = v;
  W->rule->params[1] = k;
  W->rule->params[2] = a;
  W->rule->n = 3;
  strcpy(W->rule->ident,"smallv");

  W->apply = codelet_apply_lookup(W);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");

  W->attr[vector_size] = v;
}

