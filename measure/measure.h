/**
 * \file measure.h
 *
 * \brief Routines to measure some metric against the execution of a WHT Plan.
 */
#ifndef MEASURE_H
#define MEASURE_H

#include "wht.h"
#include "stat.h"

typedef void (*measure_init_fp)(char *metric);
typedef stat_unit (*measure_call_fp)(Wht *W, wht_value *x, char *metric);
typedef void (*measure_done_fp)();


/**
 * \brief Struct to register a new extension
 *
 */
struct measure_extension
{
  char * name;
 		/**< Identifier associated with extension. */
  measure_init_fp init;
 		/**< Function to initialize the extension. */
  measure_call_fp test;
 		/**< Function to perform a test run without actually 
 		     calling \ref Wht::apply. Used for calibration. */
  measure_call_fp call;
		/**< Function to actually perform measurement.  Should call \ref Wht::apply. */
  measure_done_fp done;
 		/**< Function to cleanup the extension. */
};

double cputime();

struct stat * measure(Wht *W, char *extension, char *metric, bool calib, size_t run,
  size_t samples);

struct stat * measure_with_z_test(Wht *W, char *extension, char *metric,  bool calib, size_t run,
  size_t initial, double alpha, double rho);

struct stat * measure_until(Wht *W, char *extension, char *metric, bool calib, size_t run,
  double time);

char * measure_extension_list();

struct measure_extension * measure_extension_find(char *extension);

#endif/*MEASURE_H*/
