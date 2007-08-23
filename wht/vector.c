#include "wht.h"
#include "codelets.h"

Wht *
wht_init_vector(int n,int v)
{
  Wht *W;
  const size_t bufsize = 16; /*apply_small%d_v%d\0*/
  char buf[bufsize]; 

  snprintf(buf,bufsize,"apply_small%d_v%d",n,v);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}

Wht *
wht_parse_vector()
{
  int n,v;
  if (wht_is_codelet("smallv")) {
    v = wht_read_int();
    wht_require('[');
    n = wht_read_int();
    wht_require(']');
    return wht_init_vector(n,v);
  } 

  return NULL;
}

