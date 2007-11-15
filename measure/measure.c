#include "measure.h"
#include "extensions.h"

extern double invnorm(double p);

struct measure_extension *
measure_extension_find(char *metric)
{
  struct measure_extension *p;

  for (p = (struct measure_extension *) measure_extensions; p->metric != NULL; p++) 
    if (strncmp(metric, p->metric, strlen(p->metric)) == 0) 
      return p;
  
  return NULL;
}

inline
void
measure_helper(Wht *W, char *metric, struct stat *stat, struct measure_extension *extension)
{
  stat_unit value;
  wht_value *x;

  x = wht_random_vector(W->N);

  value = extension->call(W,x);
  stat->value = value;
  stat_update(stat);

  free(x);
}

struct stat *
measure(Wht *W, char *metric)
{
  struct measure_extension *extension;
  struct stat *stat;

  stat = stat_init();

  extension = measure_extension_find(metric);

  if (extension == NULL) 
    wht_error("No extension registered for %s\n", metric);

  extension->init(metric);

  measure_helper(W, metric, stat, extension);

  extension->done();

  return stat;
}

struct stat *
measure_with_z_test(Wht *W, char *metric, size_t initial, double alpha, double rho)
{
  struct measure_extension *extension;
  struct stat *stat;
  size_t i;
  double z;

  stat = stat_init();

  extension = measure_extension_find(metric);

  if (extension == NULL) 
    wht_error("No extension registered for %s\n", metric);

  z = invnorm(alpha);

  extension->init(metric);

  for (i = 0; i < initial; i++)
    measure_helper(W, metric, stat, extension);

  while (stat_sig_sample(stat, z, rho) > stat->samples)
    measure_helper(W, metric, stat, extension);

  extension->done();

  return stat;
}

#if 0
struct stat * 
measure_until(Wht *W, char *metric, double time)
{

}

struct stat * 
measure_at_least(Wht *W, char *metric, size_t times)
{

}
#endif
