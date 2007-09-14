#include "wht.h"
#include "codelets.h"

void
wht_guard_interleave(Wht *W, size_t right)
{
  if (W->nILNumber > right)
    wht_error("collective size of right most trees must be >= %zd", W->nILNumber);
}

Wht *
wht_init_interleave(size_t n, size_t k)
{
  Wht *W;
  const size_t bufsize = 20; /*apply_small%2d_il%2d\0*/
  char buf[bufsize]; 

  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  snprintf(buf,bufsize,"apply_small%zd_il%zd",n,k);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);
  W->guard     = wht_guard_interleave;

  W->nILNumber = k;

  if (W->apply == NULL)
    wht_error("could not find codelet %s", buf);

  return W;  
}

