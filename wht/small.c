/**
 * \file wht/small.c
 *
 */

#include "wht.h"
#include "codelets.h"

Wht *
small_init(size_t n)
{
  Wht *W;

  W            = null_init(n, "small");
  W->apply     = NULL; /* Ensure that null_apply is overridden */
  W->apply     = codelet_apply_lookup(W);

  if (n > WHT_MAX_UNROLL)
    error_msg_set(W, "not configured for unrolled codelets of size %zd", n);

  if (W->apply == NULL)
    error_msg_set(W, "could not find codelet");

  return W;  
}

