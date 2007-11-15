#ifndef MEASURE_H
#define MEASURE_H

#include "wht.h"
#include "stat.h"

typedef void (*measure_init_fp)(char *metric);
typedef stat_unit (*measure_call_fp)(Wht *W, wht_value *x);
typedef void (*measure_done_fp)();

struct measure_extension
{
  char * metric;
  measure_init_fp init;
  measure_call_fp call;
  measure_done_fp done;
};

struct stat * measure(Wht *W, char *metric);

struct stat * measure_with_z_test(Wht *W, char *metric, size_t initial, double alpha, double rho);

struct stat * measure_until(Wht *W, char *metric, double time);

struct stat * measure_at_least(Wht *W, char *metric, size_t times);

struct measure_extension * measure_extension_find(char *metric);

#endif/*MEASURE_H*/
