/* Small WHT
   ---------
   Unrolled building blocks from the library libwht.a,
   which are generated by whtgen.
*/

#include "wht.h"
#include "codelets.h"

char *
small_to_string(Wht *W)
{
  const size_t bufsize = 10; /*small[%2d]\0*/
  char *buf;

  buf = i_malloc(sizeof(char) * bufsize);
  snprintf(buf, bufsize, "small[%zd]", W->n);
  return buf;
}

Wht *
small_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  if (n > WHT_MAX_UNROLL)
    wht_error("not configured for unrolled codelets of size %zd", n);

  W            = codelet_init(n, name);
  W->apply     = wht_get_codelet(n, name, params, np);
  W->to_string = small_to_string;

  if (W->apply == NULL)
    wht_error("could not find codelet %s", W->to_string(W));

  return W;  
}

