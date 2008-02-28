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
 * \file papi_utils.c
 * \author Michael Andrews
 *
 * \brief PAPI extension to measurement subsystem.
 *
 * This code needs the PAPI library 3.2.1 or later to be installed:
 *   http://icl.cs.utk.edu/papi/
 */


/** \todo Implement PAPI_DEBUG flag to encapsulate PAPI functions that break
 *        valgrind.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <papi.h>

#include "papi_utils.h"

#define papi_eprintf(format, args...)  \
  {\
    fprintf (stderr, format , ## args); \
    exit(2); \
  }

void
papi_init() 
{
  int max;

  /* Check PAPI sanity */
  if (PAPI_VER_CURRENT != PAPI_library_init(PAPI_VER_CURRENT))
    papi_eprintf("PAPI_library_init error.\n");

  max = PAPI_num_counters();
  PAPI_reset(max);
}

void
papi_set_events(char *metric)
{
  const size_t n = 1;

  int max;
  long_long *papi_tmp;
  int papi_events[1];
  int code;

  max = PAPI_num_counters();

  if (n > max)
    papi_eprintf("Too many counters requested.\n");

  papi_tmp = malloc(sizeof(*papi_tmp) * n);

  PAPI_reset(max);

  PAPI_stop_counters(papi_tmp, n);

  if (PAPI_event_name_to_code(metric, &code) != PAPI_OK)
    papi_eprintf("Unknown PAPI event %s.\n", metric);

  if (code == 0)
    papi_eprintf("Unknown PAPI event %s.\n", metric);

  papi_events[0] = code;

  PAPI_start_counters(papi_events, n);

  if (PAPI_read_counters(papi_tmp, n) != PAPI_OK)
    papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);

  free(papi_tmp);
}

inline 
void 
papi_reset(size_t n) 
{
  long_long *papi_tmp;

  papi_tmp = malloc(sizeof(*papi_tmp) * n);

  if (PAPI_read_counters(papi_tmp, n) != PAPI_OK)
    papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);

  free(papi_tmp);
}

double
papi_generic_call()
{
  long_long tmp[1];

  /* Read and reset the counters.
   * The commented out conditional affects the reading of the performance
   * counters, but might be good for debugging.
   * NOTE: PAPI_accum_counters does not work properly.
   * */ 
#if 0
    if (PAPI_read_counters(tmp, 1) != PAPI_OK)
      papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);
#else
    PAPI_read_counters(tmp, 1);
#endif
    return ((double) tmp[0]);
}


measure_call_fp
papi_prep(char *metric)
{
  papi_set_events(metric);
  papi_reset(1);
  return &papi_generic_call;
}

/** \todo Add PAPI native event sets */
char **
papi_list()
{
  int i, j;
  size_t n;
  char **list;
  PAPI_event_info_t info;

  papi_init();

  i = PAPI_PRESET_MASK;
  n = 0;
  do {
    if (PAPI_get_event_info(i, &info) == PAPI_OK) 
      n++;
  } while (PAPI_enum_event(&i, PAPI_PRESET_ENUM_AVAIL) == PAPI_OK);

  list = malloc(sizeof(*list) * (n+1));

  i = PAPI_PRESET_MASK;
  j = 0;
  do {
    if (PAPI_get_event_info(i, &info) == PAPI_OK) {
#if 0
      /* How to get the description of an event in PAPI */
      len = strlen(info.symbol) + strlen(info.long_descr) + 6; /* X -- Y\n\0 */
      list[j] = malloc(sizeof(char) * len);
      snprintf(list[j], len, "%s -- %s", info.symbol, info.long_descr);
#endif
      list[j] = strdup(info.symbol);
      j++;
    }
  } while (PAPI_enum_event(&i, PAPI_PRESET_ENUM_AVAIL) == PAPI_OK);

  list[j] = NULL;

  return list;
}

void 
papi_done()
{
  // Empty
}

#undef papi_eprintf

