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

  stat->mean      = stat->last;
  stat->samples   = 1;

  free(x);

  return stat;
}

struct stat *
stat_init()
{
  struct stat *stat;
  stat = malloc(sizeof(*stat));

  stat->last     = 0.0;
  stat->mean     = 0.0;
  stat->stdev    = 0.0;
  stat->sum      = 0.0;
  stat->samples  = 0;

  return stat;
}

char * 
stat_to_string(struct stat *stat, bool all)
{
  char *buf;

  buf = malloc(sizeof(char) * 32);
  sprintf(buf, "%Lg", stat->mean);

  return buf;
}

struct measure_extension *
measure_extension_find(char *metric)
{
  struct measure_extension *p;

  for (p = (struct measure_extension *) measure_extensions; p->metric != NULL; p++) 
    if (strncmp(metric, p->metric, strlen(p->metric)) == 0) 
      return p;
  
  return NULL;
}
