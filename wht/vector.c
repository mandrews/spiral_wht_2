#include "wht.h"
#include "codelets.h"

extern void small_interleave_transform(Wht *W);

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
small_right_vector_transform(Wht *W)
{
  size_t v;

  v = W->params[0];

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
small_vector_transform(Wht *W)
{
  /** 
   * \todo Check assumptions about number of parameters
   */
  size_t v, k, a;

  v = W->params[0];
  k = W->params[1];
  a = W->params[2];

  if (v != WHT_VECTOR_SIZE)
    return error_msg_set(W, "not configured for vectors of size %d",v);

  if ((a == 1) && (! rightmost_tree(W)))
    return error_msg_set(W, "must be in rightmost tree of plan");

  /** 
   * \todo Check and test (size * interleave_by) > vector_size.
   * This only occurs with vectors of size >= 4  
   */

  W->params[0] = k;
  W->params[1] = UNSET_PARAMETER;
  W->params[2] = UNSET_PARAMETER;
  W->n_params = 1;
  free(W->name);
  W->name = strdup("smallil");

  small_interleave_transform(W);

  W->params[0] = v;
  W->params[1] = k;
  W->params[2] = a;
  W->n_params = 3;
  free(W->name);
  W->name = strdup("smallv");

  W->apply = codelet_apply_lookup(W);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");

  W->attr[vector_size] = v;
}

