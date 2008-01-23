/**
 * \file stat.c
 *
 * \brief Implementation of statistics data structure.
 */

#include "stat.h"

#include <stdio.h>
#include <math.h>

struct stat *
stat_init()
{
  struct stat *s;
  s = malloc(sizeof(*s));

  s->value    = 0.0;
  s->mean     = 0.0;
  s->stdev    = 0.0;
  s->sum      = 0.0;
  s->samples  = 0;

  return s;
}

void
stat_free(struct stat *s)
{
  free(s);
}

char * 
stat_to_string(struct stat *s, bool all)
{
  /** \todo Replace this with call to ftoa or lgtoa */
  const size_t bufsize = 255;
  char *buf;

  buf = malloc(sizeof(char) * bufsize);
  if (all)
    snprintf(buf, bufsize, "%Lg %Lg %zd", s->mean, sqrtl(s->stdev), s->samples);
  else
    snprintf(buf, bufsize, "%Lg", s->mean);

  return buf;
}

/**
 * Adapted from Algorithm III:
 *  http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
 * circa 2007
 *
 *  "A numerically stable algorithm is given below. It also computes the mean.
 *  This algorithm is due to Knuth, who cites Welford."
 */
void
stat_update(struct stat *s)
{
  stat_unit delta;

  s->samples++;

  delta = s->value - s->mean;

  s->mean += delta / ((stat_unit) s->samples);
  s->sum  += (delta * s->value) - (delta * s->mean);

  if (s->samples > 2) 
    s->stdev = s->sum / ((stat_unit) s->samples);
  else 
    s->stdev = s->mean;
}

/**
 * Statisical significance metric adapted from:
 *
 * The Art of Computer Performance Analysis by Raj Jain, p. 217
 * 
 * \@BOOK{Jain:1991,                                      \n
 *  AUTHOR = {Raj Jain},                                  \n
 *  TITLE = {{The Art of Computer Performance Analysis}}, \n
 *  PUBLISHER = {John Wiley \& Sons},                     \n
 *  YEAR = 1991                                           \n
 * }
 */
size_t
stat_sig_sample(struct stat *s, stat_unit z, stat_unit rho)
{
  size_t samples;
  stat_unit tmp;

  tmp = s->mean * s->mean;
  tmp = ceil(rho * rho * tmp);

  samples = (ceil(z * z * s->stdev) / tmp) + 1;

  return samples;
}

