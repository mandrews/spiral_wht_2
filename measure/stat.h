#ifndef STAT_H
#define STAT_H

#include <stdlib.h>

typedef long double stat_unit;

struct stat {
  stat_unit value;
  stat_unit mean;
  stat_unit stdev;
  stat_unit sum;
  size_t samples;
};

struct stat * stat_init();

void stat_free(struct stat *stat);

char * stat_to_string(struct stat *stat, bool all);

void stat_update(struct stat *stat);

size_t stat_sig_sample(struct stat *stat, stat_unit z, stat_unit rho);

#endif/*STAT_H*/
