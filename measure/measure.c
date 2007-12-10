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
measure_helper(Wht *W, char *metric, struct stat *stat, bool calib, struct measure_extension *extension)
{
  stat_unit value, overhead;
  wht_value *x;

  x = wht_random_vector(W->N);
  overhead = 0.0;
  value = extension->call(W,x,metric);

  if (calib)
    overhead = extension->test(W,x,metric);

  stat->value = value - overhead;

  stat_update(stat);

  free(x);
}

struct stat *
measure(Wht *W, char *name, char *metric, bool calib, 
  size_t samples)
{
  struct measure_extension *extension;
  struct stat *stat;
  size_t i;

  stat = stat_init();

  extension = measure_extension_find(name);

  if (extension == NULL) 
    wht_error("No extension registered for %s\n", metric);

  extension->init(metric);

  for (i = 0; i < samples; i++)
    measure_helper(W, metric, stat, calib, extension);

  extension->done();

  return stat;
}

struct stat *
measure_with_z_test(Wht *W, char *name, char *metric, bool calib, 
  size_t initial, double alpha, double rho)
{
  struct measure_extension *extension;
  struct stat *stat;
  size_t i;
  double z;

  stat = stat_init();

  extension = measure_extension_find(name);

  if (extension == NULL) 
    wht_error("No extension registered for %s\n", metric);

  z = invnorm(alpha);

  extension->init(metric);

  for (i = 0; i < initial; i++)
    measure_helper(W, metric, stat, calib, extension);

  while (stat_sig_sample(stat, z, rho) > stat->samples)
    measure_helper(W, metric, stat, calib, extension);

  extension->done();

  return stat;
}

struct stat * 
measure_until(Wht *W, char *name, char *metric, bool calib, 
  double time)
{
  struct measure_extension *extension;
  struct stat *stat;
  double t0, tk;

  stat = stat_init();

  extension = measure_extension_find(name);

  if (extension == NULL) 
    wht_error("No extension registered for %s\n", metric);

  extension->init(metric);

  t0 = 0.0;
  tk = 0.0;
  while (t0 < time) {
    t0 = cputime();
    measure_helper(W, metric, stat, calib, extension);
    tk += cputime() - t0;
  }

  extension->done();

  return stat;
}
