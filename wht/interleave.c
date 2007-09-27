#include "wht.h"

void
interleave_guard(Wht *W, size_t right)
{
  if (W->attr[interleave_by] > right) {
    wht_error("collective size of right most trees must be >= %d", 
      W->attr[interleave_by]);
  }
}

Wht *
interleave_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  size_t k;

  k = params[0];

  W            = small_init(name, n, params, np);
  W->guard     = interleave_guard;
  W->nk        = k; /* XXX: Right stride parameter */

  if (k > WHT_MAX_INTERLEAVE)
    wht_error("not configured for codelets of size %zd interleaved by %zd", n, k);

  W->attr[interleave_by] = k;

  return W;  
}

