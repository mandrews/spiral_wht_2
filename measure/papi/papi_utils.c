/*
 * File: papi_utils.c
 * Author: Michael Andrews <mjand@drexel.edu>
 *
 * This code needs the PAPI library 3.2.1 or later to be installed:
 *   http://icl.cs.utk.edu/papi/
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
papi_setup() 
{
  int max;

  /* Check PAPI sanity */
  if (PAPI_VER_CURRENT != PAPI_library_init(PAPI_VER_CURRENT))
    papi_eprintf("PAPI_library_init error.\n");

  max = PAPI_num_counters();
  PAPI_reset(max);
}

void
papi_get_events(const char *input, int **papi_events, size_t *n) 
{
  int i, m, code;
  char *buf, *tmp, *token, *copy;

  char prefix[] = "PAPI_";

  /* Check PAPI sanity */
  if (PAPI_VER_CURRENT != PAPI_library_init(PAPI_VER_CURRENT))
    papi_eprintf("PAPI_library_init error.\n");

  if (input == NULL)
    papi_eprintf("Cannot pass events as NULL.\n");

  char delims[] = ", ";

  copy  = malloc(sizeof(char) * (strlen(input) + 1));
  token = malloc(sizeof(char) * (strlen(input) + 1));

  strcpy(copy, input);

  /* First pass to count occurances of delimiter in input */
  for (i = 0, tmp = copy;  ; i++, tmp = NULL) {
    token = strtok(tmp, delims);
    if (token == NULL) break;
  }

  *n = i;

  *papi_events = malloc(sizeof(**papi_events) * (*n));

  strncpy(copy, input, strlen(input));

  /* Second pass initialize events from input */
  for (i = 0, tmp = copy;  ; i++, tmp = NULL) {
    token = strtok(tmp, delims);
    if (token == NULL) break;

    m = strlen(prefix) + strlen(token);
    buf = malloc((m+1) * sizeof(char));
    strcpy(buf, prefix);
    strcpy(buf + strlen(prefix), token);

    if (PAPI_event_name_to_code(buf, &code) != PAPI_OK)
      papi_eprintf("Unknown PAPI event %s.\n", buf);

    if (code == 0)
      papi_eprintf("Unknown PAPI event %s.\n", buf);

    (*papi_events)[i] = code;

    free(buf);
  }

  free(copy);
  free(token);
}

void
papi_set_events(int *papi_events, size_t n) 
{
  int max;
  long_long *papi_tmp;

  max = PAPI_num_counters();

  if (n > max)
    papi_eprintf("Too many counters requested.\n");

  papi_tmp = malloc(sizeof(*papi_tmp) * n);

  PAPI_reset(max);

  PAPI_stop_counters(papi_tmp, n);
  PAPI_start_counters(papi_events, n);

  if (PAPI_read_counters(papi_tmp, n) != PAPI_OK)
    papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);

  free(papi_tmp);
}

inline void 
papi_reset(size_t n) 
{
  long_long *papi_tmp;

  papi_tmp = malloc(sizeof(*papi_tmp) * n);

  if (PAPI_read_counters(papi_tmp, n) != PAPI_OK)
    papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);

  free(papi_tmp);
}

stat_unit
papi_test(Wht *W, wht_value *x, char *metric)
{
  long_long tmp[1];
  codelet_apply_fp apply;

  apply = W->apply;
  W->apply = null_apply;

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

  wht_apply(W,x);

  /* Read and reset the counters.
   * This conditional should NOT effect the reading of the performance
   * counters.
   * */
  if (PAPI_read_counters(tmp, 1) != PAPI_OK)
    papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);

  W->apply = apply;

  return (stat_unit) tmp[0];
}

stat_unit
papi_call(Wht *W, wht_value *x, char *metric)
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
    wht_apply(W,x);
  /* Read and reset the counters.
   * This conditional should NOT effect the reading of the performance
   * counters.
   * */
  if (PAPI_read_counters(tmp, 1) != PAPI_OK)
    papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);

  return (stat_unit) tmp[0];
}


void 
papi_init(char *metric)
{
  int *papi_events;
  size_t papi_event_total;

  papi_setup();

  /* This allocates papi_events */
  papi_get_events(metric, &papi_events, &papi_event_total);
  papi_set_events(papi_events, 1);
}

void 
papi_done()
{
  // Empty
}

#undef papi_eprintf

