#include "wht.h"
#include "codelets.h"

void
interleave_guard(Wht *W, size_t right)
{
  if (W->attr[interleave_by] > right) {
    wht_error("collective size of right most trees must be >= %zd", 
      W->attr[interleave_by]);
  }
}


char *
interleave_to_string(Wht *W)
{
  const size_t bufsize = 20; /*smallil(%2d)[%2d]\0*/
  char *buf;

  buf = wht_malloc(sizeof(char) * bufsize);
  snprintf(buf, bufsize, "smallil(%zd)[%zd]", W->attr[interleave_by], W->n);
  return buf;
}

Wht *
wht_init_interleave(size_t n, int params[], size_t np)
{
  Wht *W;
  const size_t bufsize = 20; /*apply_small%2d_il%2d\0*/
  char buf[bufsize]; 
  size_t k;

  /* TODO check size of params array */

  k = params[0];

  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  snprintf(buf,bufsize,"apply_small%zd_il%zd",n,k);

  W            = wht_init_codelet(n);
  W->apply     = wht_get_codelet(buf);
  W->guard     = interleave_guard;
  W->to_string = interleave_to_string;

  W->attr[interleave_by] = k;

  if (W->apply == NULL)
    wht_error("could not find codelet %s", buf);

  return W;  
}

