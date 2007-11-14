#include "stat.h"

#include <stdio.h>
#include <math.h>

struct stat *
stat_init()
{
  struct stat *stat;
  stat = malloc(sizeof(*stat));

  stat->value    = 0.0;
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

/* Adapted from Algorithm III:
 *  http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
 * circa 2007
 *
 *  "A numerically stable algorithm is given below. It also computes the mean.
 *  This algorithm is due to Knuth, who cites Welford."
 */
void
stat_update(struct stat *stat)
{
  stat_unit delta;

  stat->samples++;

  delta = stat->value - stat->mean;

  stat->mean += delta / ((stat_unit) stat->samples);
  stat->sum  += (delta * stat->value) - (delta * stat->mean);

  if (stat->samples > 2) 
    stat->stdev = stat->sum / ((stat_unit) stat->samples);
  else 
    stat->stdev = stat->mean;
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
bool
stat_good_sample(struct stat *stat, stat_unit zeta, stat_unit rho)
{
  size_t samples;
  stat_unit tmp;

  tmp = stat->mean * stat->mean;
  tmp = ceil(rho * rho * tmp);

  samples = (ceil(zeta * zeta * stat->stdev) / tmp) + 1;

  return stat->samples >= samples;
}

