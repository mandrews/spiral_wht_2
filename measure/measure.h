/*
 * Copyright (c) 2007 Drexel University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/**
 * \file measure.h
 * \author Michael Andrews
 *
 * \brief Routines to measure some metric against the execution of a WHT Plan.
 */
#ifndef MEASURE_H
#define MEASURE_H

/** 
 * \todo Support for printing long descriptions of events.
 */

#include "wht.h"
#include "stat.h"

typedef void (*measure_init_fp)();
typedef void (*measure_done_fp)();
typedef double (*measure_call_fp)();
typedef char ** (*measure_list_fp)();
typedef measure_call_fp (*measure_prep_fp)(char *metric);

/**
 * \brief Struct to register a new extension
 */
struct measure_extension
{
  char * name;
 		/**< Identifier associated with extension. */
  measure_init_fp init;
 		/**< Function to initialize the extension. */
  measure_list_fp list;
  measure_prep_fp prep;
  measure_done_fp done;
 		/**< Function to cleanup the extension. */
};

/**
 * \brief Get the wall clock cpu time in microseconds.
 * \return		Time in microseconds
 */
double usec();

double nsec();

double cycles();

void measure_extension_print(FILE *fd);

/**
 * \brief Deterministic measurement.
 *
 * Simple measurement method but does not account for statistical significance.
 *
 * \param W						WHT plan to be measured
 * \param	metric			Metric to measure (NULL implies microseconds)
 * \param	calib				Calibrate the measurement?
 * \param run					Number of runs to average over for a single sample point
 * \param samples			Number of samples 
 * \return						Statistics collected over sample
 */
struct stat * measure(Wht *W, char *metric, bool calib, size_t run,
  size_t samples);

/**
 * \brief Non deterministic timed measurement.  Collect measurements
 * statistical significance is acheived.
 *
 * Useful measurement method but is not guarenteed to terminate.
 *
 * \param W						WHT plan to be measured
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
struct stat * measure_with_z_test(Wht *W, char *metric,  bool calib, size_t run,
  size_t initial, double alpha, double rho);

/**
 * \brief Deterministic timed measurement.  Collect measurements until walk
 * clock time threshold is reached.
 *
 * Useful measurement method but does not account for statistical significance.
 *
 * \param W						WHT plan to be measured
 * \param	metric			Metric to measure (NULL implies microseconds)
 * \param	calib				Calibrate the measurement?
 * \param run					Number of runs to average over for a single sample point
 * \param time				Collect measurements until time has elapsed
 * \return						Statistics collected over elapsed time
 */
struct stat * measure_until(Wht *W, char *metric, bool calib, size_t run,
  double time);

#endif/*MEASURE_H*/
