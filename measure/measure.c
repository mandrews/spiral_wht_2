#include "measure.h"
#include "extensions.h"

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
stat_unit
measure_helper(Wht *W, char *metric, struct measure_extension *extension)
{
  stat_unit value;
  wht_value *x;

  x = wht_random_vector(W->N);

  value = extension->call(W,x);

  free(x);

  return value;
}

struct stat *
measure(Wht *W, char *metric)
{
  struct measure_extension *extension;
  struct stat *stat;
  stat_unit value;

  stat = stat_init();

  extension = measure_extension_find(metric);

  if (extension == NULL) 
    wht_error("No extension registered for %s\n", metric);

  extension->init(metric);

  value = measure_helper(W, metric, extension);

  extension->done();

  stat->value = value;

  stat_update(stat);

  return stat;
}

