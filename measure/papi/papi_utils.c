/*
 * File: papi_utils.c
 * Author: Michael Andrews <mjand@drexel.edu>
 *
 * This code needs the PAPI library 3.2.1 or later to be installed:
     http://icl.cs.utk.edu/papi/
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/resource.h>
#include <string.h>
#include <math.h>

#include <papi.h>
#include "papi_utils.h"

double invnorm(double a);

#define papi_min(a,b) \
	(a <= 0) ? b : ((b <= 0) ? a : ( (a < b) ? a : b ) )

#define papi_eprintf(format, args...)  \
  {\
    fprintf (stderr, format , ## args); \
    exit(2); \
  }

static BASE cache_dummy[DUMMY_SIZE];

volatile BASE cache_sink;

inline
double
cputime() {
  struct rusage rus;

  getrusage (RUSAGE_SELF, &rus);
  return ((double) rus.ru_utime.tv_sec) * 1e6 + ((double) rus.ru_utime.tv_usec);
}

void
papi_init() {
  int max;

  /* Check PAPI sanity */
  if (PAPI_VER_CURRENT != PAPI_library_init(PAPI_VER_CURRENT))
    papi_eprintf("PAPI_library_init error.\n");

  max = PAPI_num_counters();
  PAPI_reset(max);
}

void
papi_get_events(const char *input, int **papi_events, size_t *n) {
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
papi_set_events(int *papi_events, size_t n) {
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

void 
papi_data_init(struct papi_data *data, size_t n) {
  int i;

  data->cycles = 0;
  
  data->tmp   = (long_long *) malloc(sizeof(long_long) * n);
  data->mean  = (long_long *) malloc(sizeof(long_long) * n);
  data->stdev = (long_long *) malloc(sizeof(long_long) * n);
  data->sum   = (long_long *) malloc(sizeof(long_long) * n);
  data->sum2  = (long_long *) malloc(sizeof(long_long) * n);
  data->samples = 0;

  for (i = 0; i < n; i++) {
    data->tmp[i]    = 0;
    data->mean[i]   = 0;
    data->stdev[i]  = 0;
    data->sum[i]    = 0;
    data->sum2[i]   = 0;
	}
}

void 
papi_data_free(struct papi_data  *data) {
  free(data->tmp);
  free(data->mean);
  free(data->stdev);
  free(data->sum);
  free(data->sum2);
}

inline void 
papi_reset(size_t n) {
	long_long *papi_tmp;

  papi_tmp = malloc(sizeof(*papi_tmp) * n);

  if (PAPI_read_counters(papi_tmp, n) != PAPI_OK)
    papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);

	free(papi_tmp);
}

inline
void 
papi_profile_start_cycles(struct papi_data *data) {
    data->cycles -= PAPI_get_virt_cyc(); 
}

inline
void 
papi_profile_start_events(struct papi_data *data, size_t n) {
    /* Read and reset the counters.
		 * The commented out conditional affects the reading of the performance
		 * counters, but might be good for debugging.
		 * NOTE: PAPI_accum_counters does not work properly.
		 * */ 
#if 0
    if (PAPI_read_counters(data->tmp, n) != PAPI_OK) 
      papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);
#else
    PAPI_read_counters(data->tmp, n);
#endif
}

inline
void 
papi_profile_stop_cycles(struct papi_data *data) {
    data->cycles += PAPI_get_virt_cyc(); 
}

inline
void 
papi_profile_stop_events(struct papi_data *data, size_t n) {
		/* Read and reset the counters.
		 * This conditional should not effect the reading of the performance
		 * counters.
		 * NOTE: PAPI_accum_counters does not work properly.
		 * */
    if (PAPI_read_counters(data->tmp, n) != PAPI_OK) 
      papi_eprintf("Problem reading counters %s:%d.\n", __FILE__, __LINE__);
}

inline void
papi_profile_helper(void (*so_init)(void), void (*so_call)(void), void (*so_free)(void), 
  struct papi_data *data, size_t n)
{
  papi_reset(n);

  //cache_clear();

  /* Warm up PAPI */
  papi_profile_start_events(data, n);
  papi_profile_stop_events(data, n);

  (*so_init)();

  papi_profile_start_events(data, n);
  (*so_call)();
  papi_profile_stop_events(data, n);

  (*so_free)();

  papi_update_stats(data,n);

  cache_guard();

}

inline void
papi_profile(void (*so_init)(void), void (*so_call)(void), void (*so_free)(void), 
  struct papi_data *data, size_t n)
{
  size_t i,done,M;

  M = INITIAL_SAMPLES;
  cache_clear();

  for (i = 0; i < M; i++) {
    papi_profile_helper(so_init, so_call, so_free, data, n);
  }

  done = 0;

  while (!done) {
    papi_profile_helper(so_init, so_call, so_free, data, n);
    done = papi_profile_done(data,n);
  }
}

/* Adapted from Algorithm III:
 *  http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
 * circa 2007
 *
 *  "A numerically stable algorithm is given below. It also computes the mean.
 *  This algorithm is due to Knuth, who cites Welford."
 */
void
papi_update_stats(struct papi_data *data, size_t n)
{
  size_t i;

  data->samples++;

#if 0
  for (i = 0; i < n; i++) {
    data->sum[i]  += data->tmp[i];
    data->sum2[i] += data->tmp[i]*data->tmp[i];
    data->mean[i]  = data->sum[i]  / (long_long) data->samples;
    data->stdev[i] = data->sum2[i] / (long_long) data->samples;
  }
#else
  long_long delta;
  for (i = 0; i < n; i++) {
    delta = data->tmp[i] - data->mean[i];
    data->mean[i] += delta/((long_long) data->samples);
    data->sum[i] += delta*data->tmp[i] - delta*data->mean[i];

    if (data->samples > 2) {
      data->stdev[i] = data->sum[i]/(((long_long) data->samples));
    } else {
      data->stdev[i] = data->mean[i];
    }
  }
#endif
}

/* Statisical significance metric adapted from:
 *
 * The Art of Computer Performance Analysis by Raj Jain, p. 217
 * 
 * @BOOK{Jain:1991,
 *  AUTHOR = {Raj Jain},
 *  TITLE = {{The Art of Computer Performance Analysis}},
 *  PUBLISHER = {John Wiley \& Sons},
 *  YEAR = 1991
 * }
 */

int
papi_profile_done(struct papi_data *data, size_t n)
{
  double z,p,a;
  size_t i, samples, good;
  long_long tmp;

  p = 0.01;
  a = 0.99;
  z = invnorm(a);

  good = 0;

  for (i = 0; i < n; i++) {
    tmp = data->mean[i] * data->mean[i];
    tmp = ceil(p * p * tmp);

    samples = (ceil(z*z*data->stdev[i]) / tmp) + 1;

    if (data->samples >= samples)
      good++;
  }

  return good == n;
}

#undef papi_eprintf
#undef papi_min

inline
void 
cache_clear()
{
	unsigned int i;
	BASE sum = 0;

  if (PAPI_UTILS_DEBUG)
    fprintf(stderr, 
      "Clear cache array size in elements of %zd bytes is %zd.\n", 
      sizeof(BASE), DUMMY_SIZE);

	for (i=0; i < DUMMY_SIZE; i++)
    cache_dummy[i] = 3;
	for (i=0; i < DUMMY_SIZE; i++) 
    sum += cache_dummy[i];

	cache_sink = sum;
}

inline 
void
cache_guard()
{
  /* NOTE:
   * This prevents the compiler from stipping away cache_sink, cache_dummy, and
   * the functionality we want from cache_clear.
   */
	int r;
	r = random() % DUMMY_SIZE;
#if 1
  FILE *fd;
  fd = fopen("/dev/null", "w");
  fprintf(fd, BASE_FSTR, cache_sink, cache_dummy[r]);
  fclose(fd);
#else
  fprintf(stderr, BASE_FSTR, cache_sink, cache_dummy[r]);
#endif
}


