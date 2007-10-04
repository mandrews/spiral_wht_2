#include "wht.h"

bool
right_vector_accept(Wht *W, Wht *parent, size_t left, size_t right)
{
  size_t i, nn;

  if (right != 1) {
    error_msg_set("smallv(k)[n] must be right most codelet in a plan");
    return false;
  }

  nn = parent->children->nn;

  for (i = 0; i < nn; i++) {
    if (parent->children->Ws[i]->children != NULL) {
      error_msg_set("smallv(k)[n] must be right most codelet in a plan");
      return false;
    }
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

  W         = small_init(name, n, params, np);
  W->accept = right_vector_accept;

  if (v >= W->N)
    wht_error("vector size %d must < size 2^(%zd)",v,n);

  return W;  
}

void
interleave_vector_guard(Wht *W, size_t right)
{
  /* TODO: size * interleave_by > vector_size */
  if (W->attr[interleave_by] > right) {
    wht_error("collective size of right most trees must be >= %d", 
      W->attr[interleave_by]);
  }
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

  W            = small_init(name, n, params, np);
  W->guard     = interleave_vector_guard;
  W->nk        = k; /* XXX: Right stride parameter */

  W->attr[interleave_by] = k;

  return W;  
}

