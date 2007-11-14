#include "measure.h"
#include "extensions.h"

#include <string.h>

struct stat *
measure(Wht *W, char *metric)
{
  struct measure_extension *extension;
  struct stat *stat;
  wht_value *x;
  measure_start_fp start;
  measure_stop_fp  stop;

  stat = stat_init();

  extension = measure_extension_find(metric);

  if (extension == NULL) 
    wht_error("No extension registered for %s\n", metric);

  x = wht_random_vector(W->N);

  extension->init(metric);

  start = extension->start;
  stop  = extension->stop;

  start(stat);
  wht_apply(W,x);
  stop(stat);

  extension->done();

  stat->mean      = stat->value;
  stat->samples   = 1;

  free(x);

  return stat;
}

#if 0
struct stat * 
measure_with_test(Wht *W, char *metric m, size_t initial, double alpha, double rho);
{
  struct measure_extension *extn;
  struct stat *stat;
  wht_value *x;

  stat = stat_init();

  extn = measure_extension_find(metric);

  if (extn == NULL) 
    wht_error("No extension registered for %s\n", metric);

  x = wht_random_vector(W->N);

  extn->init(metric);
}
#endif

struct measure_extension *
measure_extension_find(char *metric)
{
  struct measure_extension *p;

  for (p = (struct measure_extension *) measure_extensions; p->metric != NULL; p++) 
    if (strncmp(metric, p->metric, strlen(p->metric)) == 0) 
      return p;
  
  return NULL;
}

