#ifndef MEASURE_H
#define MEASURE_H

#include "wht.h"

typedef long double stat_unit;

struct stat {
  stat_unit *last;
  stat_unit *mean;
  stat_unit *stdev;
  stat_unit *sum;
  size_t samples;
};

enum metric { USEC = 0, CYCLES, INSTRUCTIONS, L1_MISSES, OTHER };

struct stat * measure(Wht *W, enum metric m);

struct stat * measure_until(Wht *W, enum metric m, double time);

struct stat * measure_n_times(Wht *W, enum metric m, size_t n);

struct stat * measure_with_sig_test(Wht *W, enum metric m, size_t initial, double alpha, double rho);

#endif/*MEASURE_H*/
