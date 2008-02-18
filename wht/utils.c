/*
 * Copyright (c) 2007 Drexel University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/**
 * \file utils.c
 * \author Michael Andrews
 *
 * \brief Misc package utilities.
 *
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

/*
 * From: http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
 */
inline
bool
i_power_of_2(int v)
{
  return (!(v & (v - 1)) && v);
}
