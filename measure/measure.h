#ifndef MEASURE_H
#define MEASURE_H

#include "wht.h"
#include "stat.h"

typedef void (*measure_init_fp)(char *metric);
typedef stat_unit (*measure_call_fp)(Wht *W, wht_value *x, char *metric);
typedef void (*measure_done_fp)();

struct measure_extension
{
  char * name;
  measure_init_fp init;
  measure_call_fp test;
  measure_call_fp call;
  measure_done_fp done;
};

double cputime();

struct stat * measure(Wht *W, char *extension, char *metric, size_t n);

struct stat * measure_with_z_test(Wht *W, char *extension, char *metric, 
  size_t initial, double alpha, double rho);

struct stat * measure_until(Wht *W, char *extension, char *metric, double time);

char * measure_extension_list();

struct measure_extension * measure_extension_find(char *extension);

#endif/*MEASURE_H*/
