#include "wht.h"
#include "codelets.h"

bool
small_accept(Wht *W)
{
  return true;
}

Wht *
small_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;

  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  W            = null_init(name, n, params, np);
  W->apply     = NULL; /* Ensure that null_apply is overridden */
  W->apply     = wht_get_codelet(n, name, params, np);
  W->accept    = small_accept;

  if (W->apply == NULL)
    wht_error("could not find codelet %s", W->to_string(W));

  return W;  
}

