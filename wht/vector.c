#include "wht.h"

bool
right_vector_accept(Wht *W)
{
  if (W->right != 1) {
    error_msg_set("smallv(%d)[%d] must be right most codelet in a plan", 
      W->attr[vector_size], W->n);
    return false;
  } 

  if (W->parent == NULL)
    return true;

  if (W->parent->right != 1) {
    error_msg_set("smallv(%d)[%d] must be right most codelet in a plan", 
      W->attr[vector_size], W->n);
    return false;
  }

  return true;
}

Wht *
right_vector_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  int v;

  if (np != 1)
    wht_error("not enough parameters for this codelet");

  v = params[0];

  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  if (v != WHT_VECTOR_SIZE)
    wht_error("not configured for vectors of size %d",v);

  W           = small_init(name, n, params, np);
  W->accept   = right_vector_accept;

  W->attr[vector_size]    = v;

  if (v >= W->N)
    wht_error("vector size %d must < size 2^(%zd)",v,n);

  return W;  
}

bool
interleave_vector_accept(Wht *W)
{
  /** \todo Check condition size * interleave_by > vector_size.
   * This * only occurs with vectors of size >= 4  
   */

  if (W->parent == NULL) {
    error_msg_set("codelet smallv(%d,%d)[%d] must be used in conjunction with splitil", 
      W->attr[vector_size],
      W->attr[interleave_by],
      W->n);
    return false;
  }

  if (strncmp("splitil", W->parent->name, strlen("splitil"))) {
    error_msg_set("codelet smallv(%d,%d)[%d] must be used in conjunction with splitil", 
      W->attr[vector_size],
      W->attr[interleave_by],
      W->n);

    return false;
  }

  if (W->parent->right != 1) {
    error_msg_set("vector codelet smallv(%d,%d)[%d] must be relatively right most in a plan",
      W->attr[vector_size],
      W->attr[interleave_by],
      W->n);
    return false;
  } 
  
  if (W->attr[interleave_by] > W->right) {
    error_msg_set("interleave factor %d must be < %d in smallv(%d,%d)[%d]", 
      W->attr[interleave_by],
      W->right,
      W->attr[vector_size],
      W->attr[interleave_by],
      W->n);
    return false;
  }

  return true;
}

Wht *
interleave_vector_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  size_t v, k;

  if (np != 2)
    wht_error("not enough parameters for this codelet");

  v = params[0];
  k = params[1];

  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  if (v != WHT_VECTOR_SIZE)
    wht_error("not configured for vectors of size %zd",v);

  if (k > (1 << WHT_MAX_INTERLEAVE))
    wht_error("not configured for codelets of size %zd interleaved by %zd", n, k);

  W           = small_init(name, n, params, np);
  W->accept   = interleave_vector_accept;

  W->attr[vector_size]    = v;
  W->attr[interleave_by]  = k;

  return W;  
}

/** \todo This should go into external extensions header */
Wht *
vector_convert(Wht *W, size_t v, size_t k);

Wht *
small_right_vector_convert(Wht *W, size_t v)
{
  int *params;
  params = i_malloc(sizeof(*params)*1);
  params[0] = v;
  return right_vector_init("smallv", W->n, params, 1);
}

Wht *
small_interleave_vector_convert(Wht *W, size_t v, size_t k)
{
  size_t kp, k_min;
  int *params;

  if (W->right < (1 << WHT_MAX_INTERLEAVE))
    k_min = W->right;
  else
    k_min = (1 << WHT_MAX_INTERLEAVE);
 
  for (kp = k; kp > k_min; kp--);

  if (kp < 2) 
    return small_init("small", W->n, NULL, 0);

  params = i_malloc(sizeof(*params)*2);
  params[0] = v;
  params[1] = kp;

  return interleave_vector_init("smallv", W->n, params, 2);
}

Wht *
split_vector_convert(Wht *W, size_t v, size_t k)
{
  size_t nn, i;
  Wht **Ws;

  nn = W->children->nn;
  Ws = i_malloc(sizeof(*Ws) *nn);

  for (i = 0; i < nn; i++) 
    Ws[i] = vector_convert(W->children->Ws[i], v, k);

  return split_interleave_init("splitil", Ws, nn, NULL, 0);
}


Wht *
vector_convert(Wht *W, size_t v, size_t k)
{
  if (W->children)
    return split_vector_convert(W, v, k);
  else if (W->N > v && (W->parent == NULL || (W->parent->right == 1 && W->right == 1)))
    return small_right_vector_convert(W, v);
  else if (W->parent == NULL || (W->parent->right == 1))
    return small_interleave_vector_convert(W, v, k);
  else
    return small_init("small", W->n, NULL, 0);
}
