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
 * \file measure.c
 * \author Michael Andrews
 *
 * \brief Metric measurement utilities.
 */

#include <unistd.h>

#include "measure.h"
#include "extensions.h"

extern double invnorm(double p);

#ifdef HAVE_USEC
#include <sys/types.h>
#include <sys/resource.h>

double
usec()
{
  struct rusage rus;

  getrusage(RUSAGE_SELF, &rus);

  return ((double) rus.ru_utime.tv_sec) * 1e6 + 
         ((double) rus.ru_utime.tv_usec);
}
#endif

#ifdef HAVE_NSEC
#include <time.h>
double
nsec()
{
  struct timespec tp;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);

  return ((double) tp.tv_sec * 1e9) + 
         ((double) tp.tv_nsec);
}
#endif/*HAVE_NSEC*/

#ifdef HAVE_RDTSC
/**
 * \see 
 * @INPROCEEDINGS{Chellappa:08,
 *  AUTHOR = {Srinivas Chellappa and Franz Franchetti and Markus P{\"u}schel},
 *  TITLE = {How To Write Fast Numerical Code: A Small Introduction},
 *  BOOKTITLE = {Summer School on Generative and Transformational Techniques in
 *               Software Engineering},
 *  PUBLISHER = {Springer},
 *  SERIES = {Lecture Notes in Computer Science},
 *  YEAR = {2008}
 * }
 */
typedef union
{ 
  unsigned long long int64;
  struct {
    unsigned int lo, hi;
  } int32;
} tsc_counter;

#define RDTSC(cpu_c)                   \
  __asm__ __volatile__ ("rdtsc" :      \
  "=a" ((cpu_c).int32.lo),             \
  "=d" ((cpu_c).int32.hi))

double
rdtsc()
{
  tsc_counter t0;
  RDTSC(t0);
  return ((double) t0.int64);
}
#endif

void builtin_init() { /* Empty */ }
void builtin_done() { /* Empty */ }

measure_call_fp
builtin_prep(char *metric) 
{ 

#ifdef HAVE_USEC
  if (strcasecmp(metric, "usec")    == 0)
    return &usec;
#endif/*HAVE_USEC*/

#ifdef HAVE_NSEC
  if (strcasecmp(metric, "nsec")    == 0)
    return &nsec;
#endif/*HAVE_NSEC*/

#ifdef HAVE_RDTSC
  if (strcasecmp(metric, "rdtsc")  == 0)
    return &rdtsc;
#endif/*HAVE_RDTSC*/

  return NULL;
}

char **
builtin_list()
{
  const size_t N = 4;

  char **list, **p;

  list = malloc(sizeof(*list) * N);

  p = list;

#ifdef HAVE_USEC
  *p = strdup("usec"); p++;
#endif/*HAVE_USEC*/

#ifdef HAVE_NSEC
  *p = strdup("nsec"); p++;
#endif/*HAVE_NSEC*/

#ifdef HAVE_RDTSC
  *p = strdup("rdtsc"); p++;
#endif/*HAVE_RDTSC*/

  *p = NULL; p++;

  return list;
}

/** \todo Make sure these omp barriers are keeping time correctly */

inline
stat_unit
test_helper(Wht *W, wht_value *x, measure_call_fp call)
{
  volatile double t0, t1;
  codelet_apply_fp apply;

  apply = W->apply;
  W->apply = null_apply;
 
  call(); /* Warm up */
  call(); /* Warm up */

  #pragma omp single
  {
    t0 = call();
  }

  wht_apply(W,x);
  #pragma omp barrier

  #pragma omp single
  {
    t1 = call();
  }

  W->apply = apply;
  return t1 - t0;
}

inline
stat_unit
call_helper(Wht *W, wht_value *x, measure_call_fp call)
{
  volatile double t0, t1;

  call(); /* Warm up */
  call(); /* Warm up */

  #pragma omp single
  {
    t0 = call();
  }

  wht_apply(W,x);
  #pragma omp barrier

  #pragma omp single
  {
    t1 = call();
  }

  return t1 - t0;
}

struct measure_extension builtin  = 
{ 
  "BUILTIN", 
  (measure_init_fp)  &builtin_init,
  (measure_list_fp)  &builtin_list,
  (measure_prep_fp)  &builtin_prep,
  (measure_done_fp)  &builtin_done
};

bool
measure_list_include(char *metric, measure_list_fp get_list)
{
  char **list, **p;
  bool found;

  found = false;

  list = get_list();

  for (p = list; *p != NULL; p++) {
    if (strcasecmp(*p, metric) == 0) {
      found = true;
      break;
    }
  }

  for (p = list; *p != NULL; p++) 
    free(*p);

  free(list);

  return found;
}

void
measure_list_print(FILE *fd, measure_list_fp get_list)
{
  char **list, **p;


  list = get_list();

  for (p = list; *p != NULL; p++) {
    fprintf(fd, "%s\n", *p);
    free(*p);
  }

  free(list);
}


struct measure_extension *
measure_extension_find(char *metric)
{
  struct measure_extension *p;

  if (measure_list_include(metric, &builtin_list))
    return &builtin;

  for (p = (struct measure_extension *) measure_extensions; p->name != NULL; p++) 
    if (measure_list_include(metric, p->list))
      return p;

  return NULL;
}

void
measure_extension_print(FILE *fd)
{
  struct measure_extension *p;

  fprintf(fd,"%s\n--------\n", "BUILTIN");
  measure_list_print(fd, &builtin_list);
  fprintf(fd,"\n");

  for (p = (struct measure_extension *) measure_extensions; p->name != NULL; p++)  {
    fprintf(fd,"%s\n--------\n", p->name);
    measure_list_print(fd, p->list);
    fprintf(fd,"\n");
  }
}

inline
void
measure_helper(Wht *W, struct stat *stat, bool calib, size_t run, measure_call_fp call)
{
  volatile stat_unit value, overhead;
  wht_value *x;
  int i;

  x = wht_random_vector(W->N);

  value = 0.0;
  for (i = 0; i <= run; i++)
    value += call_helper(W, x, call);

  overhead = 0.0;
  if (calib)
    for (i = 0; i <= run; i++)
      overhead += test_helper(W, x, call);

  stat->value = (value - overhead) / (run*1.0);
  stat_update(stat);
  stat->samples += run;

  free(x);
}

struct stat *
measure(Wht *W, char *metric, bool calib, size_t run,
  size_t samples)
{
  struct stat *stat;
  size_t i;
  measure_call_fp call;
  struct measure_extension *extension;

  extension = measure_extension_find(metric);

  if (extension == NULL) {
    fprintf(stderr, "No extension registered for %s\n", metric);
    return NULL;
  }

  stat = stat_init();

  extension->init();

  call = extension->prep(metric);

  for (i = 0; i < samples; i++)
    measure_helper(W, stat, calib, run, call);

  extension->done();

  return stat;
}

struct stat *
measure_with_z_test(Wht *W, char *metric, bool calib, size_t run,
  size_t initial, double alpha, double rho)
{
  struct stat *stat;
  size_t i;
  double z;
  measure_call_fp call;
  struct measure_extension *extension;

  extension = measure_extension_find(metric);

  if (extension == NULL) {
    fprintf(stderr, "No extension registered for %s\n", metric);
    return NULL;
  }

  stat = stat_init();

  extension->init();

  call = extension->prep(metric);

  z = invnorm(alpha);

  for (i = 0; i < initial; i++)
    measure_helper(W, stat, calib, run, call);

  while (stat_sig_sample(stat, z, rho) > stat->samples) 
    measure_helper(W, stat, calib, run, call);

  extension->done();

  return stat;
}

struct stat * 
measure_until(Wht *W, char *metric, bool calib, size_t run,
  double tn)
{
  struct stat *stat;
  double t0, tk;
  measure_call_fp call;
  struct measure_extension *extension;

  extension = measure_extension_find(metric);

  if (extension == NULL) {
    fprintf(stderr, "No extension registered for %s\n", metric);
    return NULL;
  }

  stat = stat_init();

  extension->init();

  call = extension->prep(metric);

  t0 = 0.0;
  tk = 0.0;
  while (tk < tn) {
    t0 = usec();
    measure_helper(W, stat, calib, run, call);
    tk += usec() - t0;
  }

  extension->done();

  return stat;
}
