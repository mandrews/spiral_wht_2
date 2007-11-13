/* 
 * File: papi_utils.h
 * Author: Michael Andrews <mjand@drexel.edu>
 *
 */
#ifndef	PAPI_UTILS_H
#define PAPI_UTILS_H

#include <papi.h>
#include "measure.h"

#define BASE double
#define BASE_FSTR "# %g %g\n"

#ifndef MAX_CACHE_SIZE_KB
#define	MAX_CACHE_SIZE_KB 1024 /* Relatively safe value */
#endif/*MAX_CACHE_SIZE_KB*/

#ifndef PAPI_UTILS_DEBUG
#define PAPI_UTILS_DEBUG 0
#endif/*PAPI_UTILS_DEBUG*/

#define DUMMY_SIZE ((MAX_CACHE_SIZE_KB * 1024)/ sizeof(BASE))

#ifndef INITIAL_SAMPLES
#define INITIAL_SAMPLES 25
#endif/*INITIAL_SAMPLES*/

/* TODO 
 * Clean up this struct
 * Add so interface struct
 * Relabel prefix to papi_util, and cull non public functions
 */
struct papi_data {
  long_long  cycles;
  long_long *value;
  long_long *mean;
  long_long *stdev;
  long_long *sum;
  size_t samples;
};

double cputime();

inline
void cache_clear();

inline
void cache_guard();

void papi_init();

void papi_get_events(const char *input, int **events, size_t *n);

void papi_set_events(int *event, size_t n);

void papi_data_init(struct papi_data *data, size_t n);

void papi_data_free(struct papi_data *data);

void papi_reset();

void papi_profile_start_cycles(struct papi_data *data);

void papi_profile_stop_cycles(struct papi_data *data);

void papi_profile_start_events(struct papi_data *data, size_t n);

void papi_profile_stop_events(struct papi_data *data, size_t n);

void papi_profile(void (*so_init)(void), void (*so_call)(void), void (*so_free)(void), 
  struct papi_data *data, size_t n, double z, double p);

int papi_profile_done(struct papi_data *data, size_t n, double z, double p);

void papi_update_stats(struct papi_data *data, size_t n);

void papi_init2(char *metric);

void papi_done();

void papi_start(struct stat *stat);
void papi_stop(struct stat *stat);

#endif /*PAPI_UTILS_H*/
