#include "wht.h"
#include "codelets.h"

Wht *
wht_init_interleave(size_t n, size_t i)
{
  Wht *W;
  const size_t bufsize = 20; /*apply_small%d_il%d\0*/
  char buf[bufsize]; 

  snprintf(buf,bufsize,"apply_small%zd_v%zd_il%zd",n,2,i);
#if 0
  snprintf(buf,bufsize,"apply_small%zd_il%zd",n,i);
#endif

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);
  W->nILNumber = i;

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}

Wht *
wht_parse_interleave()
{
  int n,i;

  if (wht_is_codelet("smallil")) {
    i = wht_read_int();
    wht_require('[');
    n = wht_read_int();
    wht_require(']');
    return wht_init_interleave(n,i);
  } 

  return NULL;
}

