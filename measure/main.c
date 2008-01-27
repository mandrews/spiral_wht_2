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
 * \file main.c
 * \author Michael Andrews
 *
 * \brief Interface to measurement subsystem.
 */

#define _GNU_SOURCE

#include "measure.h"

#include <getopt.h>
#include <string.h>
#include <stdio.h>

static void
usage() 
{
  char *extensions;

  printf("Usage: wht_measure -w PLAN [OPTIONS]\n");
  printf("Measure PLAN from stdin or by argument.\n");
  printf("    -h            Show this help message.\n");
  printf("    -v            Show build information.\n");
  printf("    -w PLAN       Measure the mean metric count of PLAN.\n");
  printf("    -e EXTN       Use Measure Extension EXTN (DEFAULT BUILTIN).\n");
  printf("    -m METRIC     Measure Metric METRIC (DEFAULT USEC).\n");
  printf("    -s            Also display the standard deviation and sample size.\n");
  printf("    -c            Calibrate and subtract overhead.\n");
  printf(" Measurement Techniques (mutually exclusive):\n");
  printf("    Average (DEFAULT)\n");
  printf("      -n SAMPLES  Accumulate average for N SAMPLES (DEFAULT 1).\n");
  printf("      -k RUN      Accumulate average in run of size K (DEFAULT 1).\n");
  printf("    Timed Average\n");
  printf("      -t TIME     Accumulate average for TIME seconds.\n");
  printf("      -k RUN      Accumulate average in run of size K (DEFAULT 1).\n");
  printf("    Z-Score\n");
  printf("      -a ALPHA    Measure sample mean with ALPHA %% confidence\n");
  printf("      -p RHO      Measure within RHO %% of sample mean\n");
  printf("      -n SAMPLES  Initial number of N SAMPLES (DEFAULT 1).\n");
  printf("      -k RUN      Accumulate average in run of size K (DEFAULT 1).\n");
  printf("\n");

  extensions = measure_extension_list();

  printf(" Available extensions: %s\n", extensions);
#ifdef  HAVE_PAPI
  printf("\n For a list of available PAPI metrics see PAPI_INSTALL_DIR/bin/papi_avail.\n");
#endif/*HAVE_PAPI*/

  free(extensions);

  exit(EXIT_FAILURE);
}

int 
main(int argc, char **argv)
{
  char *plan, *extn, *metric;
  int c, n, k;
  double a, p, t;
  bool all, calib;
  size_t len;

  plan = NULL;
  extn = NULL;
  metric = NULL;
  all = false;
  calib = false;
  a = INFINITY;
  p = INFINITY;
  t = INFINITY;
  n = 1;
  k = 1;

  while ((c = getopt (argc, argv, "hvw:e:sa:p:m:t:n:k:c")) != -1)
    switch (c) {
      case 'w':
        plan = strdup(optarg);
        break;
      case 'e':
        extn = optarg;
        break;
      case 'm':
        metric = optarg;
        break;
      case 's':
        all = true;
        break;
      case 'a':
        a = atof(optarg);
        a /= 100.0;
        break;
      case 'p':
        p = atof(optarg);
        p /= 100.0;
        break;
      case 't':
        t = atof(optarg);
        t *= 1e6;
        break;
      case 'n':
        n = atoi(optarg);
        break;
      case 'k':
        k = atoi(optarg);
        break;
      case 'c':
        calib = true;
        break;
      case 'v':
        wht_info();
        exit(1);
      case 'h':
        usage();
      default:
        usage();
    }

  if (plan == NULL)
    getline(&plan, &len, stdin);

  if (plan == NULL)
    usage();

  if ((a != INFINITY && p == INFINITY) || (a == INFINITY && p != INFINITY)) {
    printf("must choose both alpha and rho\n\n");
    usage();
  }

  if (a != INFINITY && p != INFINITY && t != INFINITY) {
    printf("measurement techniques are mutually exclusive\n\n");
    usage();
  }

  Wht *W;
  struct stat *s;
  char *buf;

  W = wht_parse(plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  if (a != INFINITY && p != INFINITY)
    s = measure_with_z_test(W, extn, metric, calib, k, n, a, p);
  else if (t != INFINITY)
    s = measure_until(W, extn, metric, calib, k, t);
  else
    s = measure(W, extn, metric, calib, k, n);

  buf = stat_to_string(s, all);

  printf("%s\n", buf);

  wht_free(W);
  stat_free(s);
  free(buf);
  free(plan);

  return 0;
}

#ifndef DOXYGEN_MAN_MODE
/**
\page wht_measure Measure WHT Plan

\section _synopsis SYNOPSIS
wht_measure -w PLAN [OPTIONS]

\section _description DESCRIPTION

Calculate a performance metric over the execution of a WHT Plan.  This can be done using 
the Average, Timed Average, or until the number of samples satisfies a Z-Score test.

\verbatim
Measure PLAN from stdin or by argument.
    -h            Show this help message.
    -v            Show build information.
    -w PLAN       Measure the mean metric count of PLAN.
    -e EXTN       Use Measure Extension EXTN (DEFAULT BUILTIN).
    -m METRIC     Measure Metric METRIC (DEFAULT USEC).
    -s            Also display the standard deviation and sample size.
    -c            Calibrate and subtract overhead.
 Measurement Techniques (mutually exclusive):
    Average (DEFAULT)
      -n SAMPLES  Accumulate average for N SAMPLES (DEFAULT 1).
      -k RUN      Accumulate average in run of size K (DEFAULT 1).
    Timed Average
      -t TIME     Accumulate average for TIME seconds.
      -k RUN      Accumulate average in run of size K (DEFAULT 1).
    Z-Score
      -a ALPHA    Measure sample mean with ALPHA % confidence
      -p RHO      Measure within RHO % of sample mean
      -n SAMPLES  Initial number of N SAMPLES (DEFAULT 1).
      -k RUN      Accumulate average in run of size K (DEFAULT 1).

 Available extensions: PAPI 

 For a list of available PAPI metrics see PAPI_INSTALL_DIR/bin/papi_avail.
\endverbatim

\section _examples EXAMPLES

These examples assume that you have configured the package --with-papi.

Measure the total number of cycles it takes to execute the small[2] codelet using PAPI.
Calibrate the measurement (takes slightly longer).  Run 20 samples, and for each sample 
run 10 times and average.  This typically decreases the standard deviation between samples but
may have the unintended consequence of interferring with the affect of the memory heirarchy on running time.

\verbatim
wht_measure -w 'small[2]' -e PAPI -m TOT_CYC -c -k 10 -n 10
432.378
\endverbatim

The USEC measurement is only accurate when the transform is fairly large. Still many more samples need to be taken.

\verbatim
wht_measure -w 'split[small[4],small[4],small[4]]' -k 20 -n 40 -c
374.335
\endverbatim

The Z-Score Test allows the number of samples to vary and guarentees an accurate result that will be within RHO of the true mean with ALPHA% confidence.  This allows the user to decrease the number of required samples and still achieve statistica significance.

\verbatim
wht_measure -w 'split[small[4],small[4],small[4]]' -k 20 -n 10 -c -a 0.5 -p 1 
375.293 
\endverbatim

*/
#endif/*DOXYGEN_MAN_MODE*/
