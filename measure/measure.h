#ifndef MEASURE_H
#define MEASURE_H

#include "wht.h"
#include "stat.h"

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

struct stat * measure_with_test(Wht *W, char *metric m, size_t initial, double alpha, double rho);
#endif

struct measure_extension * measure_extension_find(char *metric);

#endif/*MEASURE_H*/
