#include "wht.h"

#include <time.h>

/* Allocation
   ==========
*/

/**
 * For now this function just ensures optimal memory alignment, could be
 * extended in future.
 */
void *
i_malloc(size_t size) 
{
  void *p __attribute__ ((aligned (16)));

  p = malloc(size);
  if (p == NULL)
    wht_exit("out of memory in malloc()");
  return p;
}

/**
 * For now this function just calls free, could be extended in future.
 */
void 
i_free(void *p) 
{
  free(p);
}

wht_value *
random_vector(size_t n)
{
  wht_value *x;
  int i;
  char state[128];
  unsigned int seed;


 /* Initializing the seed */
 seed = time(NULL) + getpid();
 initstate(seed, state, 128);
  
  x = (wht_value *) i_malloc(n * sizeof(wht_value));
  for (i = 0; i < n; i++) 
    x[i] = ((wht_value) (rand() + 1.0L))/RAND_MAX;

  return x;
}


#define pkg_max(a,b) (a > b ? a : b)

wht_value
max_norm(const wht_value *x, const wht_value *y, size_t N)
{
  int i;
  wht_value norm;
  

  norm = WHT_STABILITY_THRESHOLD*1e-1;
#if   WHT_FLOAT==1
  for (i = 0; i < N; i++) 
    norm = pkg_max(fabsf(x[i] - y[i]),norm);
#endif/*WHT_FLOAT*/

#if   WHT_DOUBLE==1
  for (i = 0; i < N; i++) 
    norm = pkg_max(fabs(x[i] - y[i]),norm);
#endif/*WHT_DOUBLE*/

  return norm;

}

#undef pkg_max

