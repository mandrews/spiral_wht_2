#include "wht.h"
#include "codelets.h"

bool
small_transform(Wht *W)
{
  return true;
}

Wht *
small_init(size_t n)
{
  Wht *W;

  if (n > WHT_MAX_UNROLL)
    erro_msg_set("not configured for unrolled codelets of size %zd", n);

  W            = null_init(n, "small");
  W->apply     = NULL; /* Ensure that null_apply is overridden */
  W->apply     = codelet_apply_lookup(n, "small", NULL, 0);
  W->transform = small_transform;

  if (W->apply == NULL)
    erro_msg_set("could not find codelet %s", W->to_string(W));

  return W;  
}

