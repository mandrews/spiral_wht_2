/* Small WHT
   ---------
   Unrolled building blocks from the library libwht.a,
   which are generated by whtgen.
*/

#include "wht.h"
#include "codelets.h"

/* NOTE: Can use dynamic loading instad of case statement */

/* Forward Declarations
   ====================
*/
Wht * wht_init_small(int n);
static void wht_free_small(Wht *W);
char * wht_to_string_small();

Wht *
parse_small()
{
  int n;
  if (wht_is_codelet("small")) {
    wht_require('[');
    n = wht_read_int();
    wht_require(']');
    return wht_init_small(n);
  } 

  return NULL;
}

Wht *
wht_init_small(int n) {
  Wht *W;
  char buf[13]; /*apply_small%d\0*/

  if (n > MAX_UNROLLED)
    wht_error("%d > %d too large", n, MAX_UNROLLED);

  snprintf(buf,13,"apply_small%d",n);

  W            = (Wht *) wht_malloc(sizeof(Wht));
  W->N         = (long) pow((double) 2, (double) n);
  W->n         = n;
  W->free      = wht_free_small;
  W->to_string = wht_to_string_small;
  W->apply     = wht_get_codelet(buf);
  W->nILNumber = 1;

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}

static void 
wht_free_small(Wht *W) 
{
  wht_free(W);
}

char *
wht_to_string_small()
{
  return NULL;
}

void
guard_small(Wht *W)
{

}
