#include "wht.h"

bool
interleave_accept(Wht *W)
{
  if (W->attr[interleave_by] > W->right) {
    error_msg_set("interleave factor %d must be < %d in smallil(%d)[%d]", 
      W->attr[interleave_by],
      W->right,
      W->attr[interleave_by],
      W->n);
    return false;
  } 
    
  return true;
}


Wht *
interleave_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  size_t k;

  k = params[0];

  W            = small_init(name, n, params, np);
  W->accept    = interleave_accept;
  W->nk        = k; /* XXX: Right stride parameter */

  if (k > (1 << WHT_MAX_INTERLEAVE))
    wht_error("not configured for codelets of size %zd interleaved by %zd", n, k);

  W->attr[interleave_by] = k;

  return W;  
}

