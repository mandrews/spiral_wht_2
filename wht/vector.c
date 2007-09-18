#include "wht.h"
#include "codelets.h"

Wht *
wht_init_right_vector(size_t n, size_t v)
{
  Wht *W;
  const size_t bufsize = 20; /*apply_small%2d_v%2d_a\0*/
  char buf[bufsize]; 

  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  if (v != WHT_VECTOR_SIZE)
    wht_error("not configured for vectors of size %zd",v);

  snprintf(buf,bufsize,"apply_small%zd_v%zd_a",n,v);

  W            = wht_init_codelet(n);

  if (v >= W->N)
    wht_error("vector size %zd must < size 2^(%zd)",v,n);

  W->apply     = wht_get_codelet(buf);

  if (W->apply == NULL)
    wht_error("could not find codelet %s", buf);

  return W;  
}

void
wht_guard_interleave_vector(Wht *W, size_t right)
{
  if (W->attr[interleave_by] > right) {
    wht_error("collective size of right most trees must be >= %zd", 
      W->attr[interleave_by]);
  }
}

Wht *
wht_init_interleave_vector(size_t n, size_t v, size_t k)
{
  Wht *W;
  const size_t bufsize = 24; /*apply_small%2d_v%2d_il%2d\0*/
  char buf[bufsize]; 

  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  if (v != WHT_VECTOR_SIZE)
    wht_error("not configured for vectors of size %zd",v);

  snprintf(buf,bufsize,"apply_small%zd_v%zd_il%zd",n,v,k);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);
  W->guard     = wht_guard_interleave_vector;

  W->attr[interleave_by] = k;

  if (W->apply == NULL)
    wht_error("could not find codelet %s", buf);

  return W;  
}

