#include "wht.h"

/* Allocation
   ==========
*/

/* avoiding allocations < 32 Byte */
void *
wht_malloc(size_t length) {
  void *p;

  /* avoid mallocs of size below 32 bytes */
  if (length < 32)
    length = 32;

  p = malloc(length);
  if (p == NULL)
    wht_error("out of memory in malloc()");
  return p;
}

void 
wht_free(void *p) {
  free(p);
}

wht_value *
wht_random(size_t n)
{
  wht_value *x;
  int i;
  
  x = (wht_value *) wht_malloc(n * sizeof(wht_value));
  for (i = 0; i < n; i++) 
    x[i] = ((wht_value) (rand() + 1.0L))/RAND_MAX;

  return x;
}


/* Auxiliary Functions 
   ===================
*/

/* is2power( <n> )
   ---------------
     checks whether the integer <n> is a 2-power >= 1.
*/
int 
is2power(long n) {
  if (n < 1)
    return 0;

  while (n % 2 == 0)
    n /= 2;

  if (n == 1)
    return 1;
  else
    return 0;
}

