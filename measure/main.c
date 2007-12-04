#include "measure.h"

#include <getopt.h>

static void
usage() 
{
  char *extensions;

  printf("Usage: wht_measure -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w PLAN   Measure the mean metric count of PLAN execution (default USEC).\n");
  printf("    -e EXTN   Use measure extension EXTN (default BUILTIN).\n");
  printf("    -m METRIC Measure metric METRIC (default USEC).\n");
  printf("    -s        Also display the standard deviation and sample size.\n");
  printf("    -c        Calibrate and subtract overhead.\n");
  printf(" Measurement Techniques (mutually exclusive):\n");
  printf("    Average (default)\n");
  printf("      -n SAMPLES  Accumulate average for N SAMPLES (default 1).\n");
  printf("    Timed Average\n");
  printf("      -t TIME     Accumulate average for TIME seconds.\n");
  printf("    Z-Score\n");
  printf("      -a ALPHA    Measure sample mean with ALPHA %% confidence\n");
  printf("      -p RHO      Measure within RHO %% of sample mean\n");
  printf("      -n SAMPLES  Initial number of N SAMPLES (default 1).\n");
  printf("\n");

  extensions = measure_extension_list();

  printf(" Available extensions: %s\n", extensions);
  printf(" For a list of available PAPI metrics see PAPI_INSTALL_DIR/bin/papi_avail.\n");

  free(extensions);

  exit(EXIT_FAILURE);
}

int 
main(int argc, char **argv)
{
  char *wht_plan, *extn, *metric;
  int c, n;
  double a, p, t;
  bool all, calib;

  wht_plan = NULL;
  extn = NULL;
  metric = NULL;
  all = false;
  calib = false;
  a = INFINITY;
  p = INFINITY;
  t = INFINITY;
  n = 1;

  while ((c = getopt (argc, argv, "hvw:e:sa:p:m:t:n:c")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
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

  if (wht_plan == NULL)
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

  W = wht_parse(wht_plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    wht_free(W);
    exit(1);
  }

  if (a != INFINITY && p != INFINITY)
    s = measure_with_z_test(W, extn, metric, calib, n, a, p);
  else if (t != INFINITY)
    s = measure_until(W, extn, metric, calib, t);
  else
    s = measure(W, extn, metric, calib, n);

  buf = stat_to_string(s, all);

  printf("%s\n", buf);

  wht_free(W);
  stat_free(s);
  free(buf);

  return 0;
}
