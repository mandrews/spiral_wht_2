/**
 * \file utils.c
 *
 * \brief Misc package utilities.
 */
#include "wht.h"

#include <math.h>

void *
i_malloc(size_t size) 
{
  /**
   * For now this function just ensures optimal memory alignment, could be
   * extended in future.
   */
  void *p __attribute__ ((aligned (16)));

  p = malloc(size);
  if (p == NULL)
    wht_exit("out of memory in malloc()");
  return p;
}

void 
i_free(void *p) 
{
  /**
   * For now this function just calls free, could be extended in future.
   */
  free(p);
}

char * 
i_itoa(int i)
{
  const size_t n = sizeof(int) * 8; 
  /** 
   * \note n should log[10](length of maximum integer)
   * This should work since log[2](x) > log[10](x) 
   */

  char *buf;
  buf = malloc(sizeof(char) * n);

  snprintf(buf, n, "%d", i);

  return buf;
}

