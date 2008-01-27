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

#include "measure.h"
#include "extensions.h"

extern double invnorm(double p);

#include <sys/types.h>
#include <sys/resource.h>

inline
double
cputime()
{
  struct rusage rus;

  getrusage(RUSAGE_SELF, &rus);

  return ((double) rus.ru_utime.tv_sec) * 1e6 + ((double) rus.ru_utime.tv_usec);
}

void builtin_init(char *metric) { /* Empty, only metric is usec */ }
void builtin_done() { /* Empty */ }

stat_unit
builtin_test(Wht *W, wht_value *x, char *metric)
{
  /* Microsecond metric */
  double t0, t1;
  codelet_apply_fp apply;

  apply = W->apply;
  W->apply = null_apply;

  t0 = cputime();
  wht_apply(W,x);
  t1 = cputime();

  W->apply = apply;
  return t1 - t0;
}

stat_unit
builtin_call(Wht *W, wht_value *x, char *metric)
{
  /* Microsecond metric */
  double t0, t1;

  t0 = cputime();
  wht_apply(W,x);
  t1 = cputime();

  return t1 - t0;
}

struct measure_extension builtin  = 
{ 
  "BUILTIN", 
  (measure_init_fp)  &builtin_init,
  (measure_call_fp)  &builtin_test,
  (measure_call_fp)  &builtin_call,
  (measure_done_fp)  &builtin_done
};

struct measure_extension *
measure_extension_find(char *name)
{
  struct measure_extension *p;

  if (name == NULL) 
    return &builtin;

  for (p = (struct measure_extension *) measure_extensions; p->name != NULL; p++) 
    if (strncmp(name, p->name, strlen(p->name)) == 0) 
      return p;
  
  return NULL;
}

char *
measure_extension_list()
{
  struct measure_extension *p;
  size_t n;
  char *buf;

  n = 0;
  for (p = (struct measure_extension *) measure_extensions; p->name != NULL; p++)
    n += strlen(p->name) + 1; /* 1 for the extra space */

  buf = malloc(sizeof(char) * n + 1); /* 1 for the \0 */

  strcpy(buf,"");

  for (p = (struct measure_extension *) measure_extensions; p->name != NULL; p++) {
    strcat(buf, p->name);
    strcat(buf, " ");
  }

  return buf;
}

inline
void
measure_helper(Wht *W, char *metric, struct stat *stat, bool calib, size_t run,
  struct measure_extension *extension)
{
  stat_unit value, overhead;
  wht_value *x;
  int i;

  x = wht_random_vector(W->N);

  value = 0.0;
  for (i = 0; i <= run; i++)
    value += extension->call(W,x,metric);

  overhead = 0.0;
  if (calib)
    for (i = 0; i <= run; i++)
      overhead += extension->test(W,x,metric);

  stat->value = (value - overhead) / (run*1.0);

  stat_update(stat);

  stat->samples += run;

  free(x);
}

struct stat *
measure(Wht *W, char *name, char *metric, bool calib, size_t run,
  size_t samples)
{
  struct measure_extension *extension;
  struct stat *stat;
  size_t i;

  stat = stat_init();

  extension = measure_extension_find(name);

  if (extension == NULL) 
    wht_exit("No extension registered for %s\n", metric);

  extension->init(metric);

  for (i = 0; i < samples; i++)
    measure_helper(W, metric, stat, calib, run, extension);

  extension->done();

  return stat;
}

struct stat *
measure_with_z_test(Wht *W, char *name, char *metric, bool calib, size_t run,
  size_t initial, double alpha, double rho)
{
  struct measure_extension *extension;
  struct stat *stat;
  size_t i;
  double z;

  stat = stat_init();

  extension = measure_extension_find(name);

  if (extension == NULL) 
    wht_exit("No extension registered for %s\n", metric);

  z = invnorm(alpha);

  extension->init(metric);

  for (i = 0; i < initial; i++)
    measure_helper(W, metric, stat, calib, run, extension);

  while (stat_sig_sample(stat, z, rho) > stat->samples) 
    measure_helper(W, metric, stat, calib, run, extension);

  extension->done();

  return stat;
}

struct stat * 
measure_until(Wht *W, char *name, char *metric, bool calib, size_t run,
  double tn)
{
  struct measure_extension *extension;
  struct stat *stat;
  double t0, tk;

  stat = stat_init();

  extension = measure_extension_find(name);

  if (extension == NULL) 
    wht_exit("No extension registered for %s\n", metric);

  extension->init(metric);

  t0 = 0.0;
  tk = 0.0;
  while (tk < tn) {
    t0 = cputime();
    measure_helper(W, metric, stat, calib, run, extension);
    tk += cputime() - t0;
  }

  extension->done();

  return stat;
}
