#include "wht.h"
#include "codelets.h"

/* Forward Declarations
   ====================
*/
Wht * wht_init_vector(int n,int v);
static void wht_free_vector(Wht *W);
char * wht_to_string_vector();

Wht *
parse_vector()
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

Wht *
wht_init_vector(int n,int v) {
  Wht *W;
  const size_t bufsize = 16; /*apply_small%d_v%d\0*/
  char buf[bufsize]; 

  if (n > MAX_UNROLLED)
    wht_error("%d > %d too large", n, MAX_UNROLLED);

  snprintf(buf,bufsize,"apply_small%d_v%d",n,v);

  W            = (Wht *) wht_malloc(sizeof(Wht));
  W->N         = (long) pow((double) 2, (double) n);
  W->n         = n;
  W->free      = wht_free_vector;
  W->to_string = wht_to_string_vector;
  W->apply     = wht_get_codelet(buf);
  W->nILNumber = 1;

  if (W->apply == NULL)
    wht_error("Could not find codelet %s.", buf);

  return W;  
}

static void 
wht_free_vector(Wht *W) 
{
  wht_free(W);
}

char *
wht_to_string_vector()
{
  return NULL;
}

void
guard_vector(Wht *W)
{

}
