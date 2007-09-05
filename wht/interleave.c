#include "wht.h"
#include "codelets.h"

Wht *
wht_init_interleave(size_t n, size_t i)
{
  Wht *W;
  const size_t bufsize = 20; /*apply_small%2d_il%2d\0*/
  char buf[bufsize]; 

  snprintf(buf,bufsize,"apply_small%zd_il%zd",n,i);
  printf("%s\n",buf);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);
  W->nILNumber = i;

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}

#if 0
Wht *
wht_init_interleave_vector(size_t n, size_t i)
{
  Wht *W;
  const size_t bufsize = 24; /*apply_small%d_v%d_il%d\0*/
  char buf[bufsize]; 

  snprintf(buf,bufsize,"apply_small%zd_v%zd_il%zd",n,4,i);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);
  W->nILNumber = i;

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}
#endif

