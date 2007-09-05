#include "wht.h"
#include "codelets.h"

Wht *
wht_init_interleave(size_t n, size_t k)
{
  Wht *W;
  const size_t bufsize = 20; /*apply_small%2d_il%2d\0*/
  char buf[bufsize]; 

  snprintf(buf,bufsize,"apply_small%zd_il%zd",n,k);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);
  W->nILNumber = k;

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}

