#ifndef MEASURE_H
#define MEASURE_H

#include "wht.h"

typedef long double stat_unit;

struct stat {
  stat_unit last;
  stat_unit mean;
  stat_unit stdev;
  stat_unit sum;
  size_t samples;
};

typedef void (*measure_init_fp)(char *metric);
typedef void (*measure_start_fp)(struct stat *stat);
typedef void (*measure_stop_fp)(struct stat *stat);
typedef void (*measure_done_fp)();

struct measure_extension
{
  char * metric;
  measure_init_fp   init;
  measure_start_fp  start;
  measure_stop_fp   stop;
  measure_done_fp   done;
};

struct stat * measure(Wht *W, char *metric);

#if 0
struct stat * measure_until(Wht *W, enum metric m, double time);

struct stat * measure_n_times(Wht *W, enum metric m, size_t n);

struct stat * measure_with_test(Wht *W, enum metric m, size_t initial, double alpha, double rho);
#endif

struct stat * stat_init();

void stat_update(struct stat *st);

bool stat_done(struct stat *st, double zeta, double rho);

char * stat_to_string(struct stat *stat, bool all);

struct measure_extension * measure_extension_find(char *metric);

#endif/*MEASURE_H*/
