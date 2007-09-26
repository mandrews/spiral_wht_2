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
wht_init_interleave(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  size_t k;

  W            = small_init(name, n, params, np);
  W->guard     = interleave_guard;

  k = params[0];

  if (k > WHT_MAX_INTERLEAVE)
    wht_error("not configured for codelets of size %zd interleaved by %zd", n, k);

  W->attr[interleave_by] = k;

  return W;  
}

