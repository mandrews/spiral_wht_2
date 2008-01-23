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
 */
struct measure_extension
{
  char * name;
 		/**< Identifier associated with extension. */
  measure_init_fp init;
 		/**< Function to initialize the extension. */
  measure_call_fp test;
 		/**< Function to perform a test run without actually 
 		     calling \ref Wht::apply. Should replace \ref Wht::apply
 				 with \ref null_apply Used for calibration. */
  measure_call_fp call;
		/**< Function to actually perform measurement.  Should call \ref Wht::apply. */
  measure_done_fp done;
 		/**< Function to cleanup the extension. */
};

/**
 * \brief Get the wall clock cpu time in microseconds.
 * \return		Time in microseconds
 */
double cputime();

/**
 * \brief Deterministic measurement.
 *
 * Simple measurement method but does not account for statistical significance.
 *
 * \param W						WHT plan to be measured
 * \param extension   Measurement extension to be used (NULL implies builtin)
 * \param	metric			Metric to measure (NULL implies microseconds)
 * \param	calib				Calibrate the measurement?
 * \param run					Number of runs to average over for a single sample point
 * \param samples			Number of samples 
 * \return						Statistics collected over sample
 */
struct stat * measure(Wht *W, char *extension, char *metric, bool calib, size_t run,
  size_t samples);

/**
 * \brief Non deterministic timed measurement.  Collect measurements
 * statistical significance is acheived.
 *
 * Useful measurement method but is not guarenteed to terminate.
 *
 * \param W						WHT plan to be measured
 * \param extension   Measurement extension to be used (NULL implies builtin)
 * \param	metric			Metric to measure (NULL implies microseconds)
 * \param	calib				Calibrate the measurement?
 * \param run					Number of runs to average over for a single sample point
 * \param initial     Initiali number of samples to collect before significance
 * 										test
 * \param alpha				Sample until \alpha % confident that sample size is
 * 										significant
 * \param rho					Sample until \alpha % confident that sample size is
 * 										within \rho % of the true mean
 * \return						Statistics collected 
 */
struct stat * measure_with_z_test(Wht *W, char *extension, char *metric,  bool calib, size_t run,
  size_t initial, double alpha, double rho);

/**
 * \brief Deterministic timed measurement.  Collect measurements until walk
 * clock time threshold is reached.
 *
 * Useful measurement method but does not account for statistical significance.
 *
 * \param W						WHT plan to be measured
 * \param extension   Measurement extension to be used (NULL implies builtin)
 * \param	metric			Metric to measure (NULL implies microseconds)
 * \param	calib				Calibrate the measurement?
 * \param run					Number of runs to average over for a single sample point
 * \param time				Collect measurements until time has elapsed
 * \return						Statistics collected over elapsed time
 */
struct stat * measure_until(Wht *W, char *extension, char *metric, bool calib, size_t run,
  double time);

/**
 * \brief List the measurement extensions registered.
 * \return						String listing of extensions.
 */
char * measure_extension_list();

/**
 * \brief Lookup a measurement extension based on it's name.
 * \param name				Measurement extension to find
 * \return						Pointer to the measure_extension
 */
struct measure_extension * measure_extension_find(char *name);

#endif/*MEASURE_H*/
