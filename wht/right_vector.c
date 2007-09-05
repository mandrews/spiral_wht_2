#include "wht.h"
#include "codelets.h"

Wht *
wht_init_right_vector(int n, int v)
{
  Wht *W;
  const size_t bufsize = 20; /*apply_small%2d_v%2d_a\0*/
  char buf[bufsize]; 

  snprintf(buf,bufsize,"apply_small%d_v%d_a",n,v);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}

