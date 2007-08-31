#include "wht.h"

#include <time.h>

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
  char state[128];
  unsigned int seed;


 /* Initializing the seed */
 seed = time(NULL) + getpid();
 initstate(seed, state, 128);
  
  x = (wht_value *) wht_malloc(n * sizeof(wht_value));
  for (i = 0; i < n; i++) 
    x[i] = ((wht_value) (rand() + 1.0L))/RAND_MAX;

  return x;
}


/* Auxiliary Functions 
   ===================
*/

/* wht_is2power( <n> )
   ---------------
     checks whether the integer <n> is a 2-power >= 1.
*/
int 
wht_is2power(long n) {
  if (n < 1)
    return 0;

  while (n % 2 == 0)
    n /= 2;

  if (n == 1)
    return 1;
  else
    return 0;
}

#define wht_max(a,b) (a > b ? a : b)

wht_value
wht_max_norm(const wht_value *x, const wht_value *y, size_t N)
{
  int i;
  wht_value norm;
  

  norm = WHT_STABILITY_THRESHOLD*1e-1;
#if   WHT_FLOAT==1
  for (i = 0; i < N; i++) 
    norm = wht_max(fabsf(x[i] - y[i]),norm);
#endif/*WHT_FLOAT*/

#if   WHT_DOUBLE==1
  for (i = 0; i < N; i++) 
    norm = wht_max(fabs(x[i] - y[i]),norm);
#endif/*WHT_DOUBLE*/

  return norm;

}

#undef wht_max
