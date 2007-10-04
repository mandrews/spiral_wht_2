#include "wht.h"
#include "papi_utils.h"

#include <getopt.h>
#include <stdbool.h>

#define DEFAULT_ALPHA (95.0)
#define DEFAULT_RHO   (05.0)

Wht *W;
wht_value *x;
long N;

void _wht_init()
{
  N = W->N;
  x = wht_random_vector(N);
}

void _wht_free()
{
  /* NOTE: this should be free'd but we are abusing the previous papi_utils
   * system into taking warm cache measurements
   */
#if 0
  wht_free(W);
  free(x);
#endif
}

void _wht_call()
{
  wht_apply(W,x);
}

double invnorm(double a);

static void
usage() 
{
  printf("Usage: wht_measure -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w PLAN   Measure the mean metric count of PLAN execution (default cycles).\n");
  printf("    -s        Also display the standard deviation and sample size.\n");
  printf("    -a ALPHA  Measure sample mean with ALPHA %% confidence (default %.2f).\n", DEFAULT_ALPHA);
  printf("    -p RHO    Measure within RHO %% of sample mean (default %.2f).\n", DEFAULT_RHO);
  printf("    -m METRIC Measure PAPI metric METRIC w/o the PAPI_ prefix (default TOT_CYC).\n");
  printf("\n");
  printf(" For a list of available PAPI metrics see PAPI_INSTALL_DIR/bin/papi_avail.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *wht_plan, *papi_metric;
  int c;
  bool stats;
  double a, z, p;

  char default_papi_metric[] = "TOT_CYC";

  /* Default parameters */
  wht_plan      = NULL;
  papi_metric   = NULL;

  stats = false;

  a = 95.0;
  p = 05.0;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:sa:p:m:")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
        break;
      case 'm':
        papi_metric = optarg;
        break;
      case 's':
        stats = true;
        break;
      case 'a':
        a = atof(optarg);
        break;
      case 'p':
        p = atof(optarg);
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

  if (a < 50.0 || a >= 100.0) {
    printf("-a ALPHA must be a real number such that (50.0 <= ALPHA < 100.0).\n");
    usage();
  }

  if (p <= 0.0 || p >= 100.0) {
    printf("-p RHO must be a real number such that (0.0 < RHO < 100.0).\n");
    usage();
  }

  struct papi_data total;
  int *papi_events;
  size_t papi_event_total;

  /* Function pointers to wht symbols */
  void (*so_init)(void);
  void (*so_call)(void);
  void (*so_free)(void);

  so_init = &_wht_init;
  so_free = &_wht_free;
  so_call = &_wht_call;

  W = wht_parse(wht_plan);

  if (wht_accept(W) != true) {
    wht_free(W);
    printf("rejected, %s\n", wht_error_msg);
    exit(1);
  }

  if (papi_metric == NULL) 
    papi_metric = default_papi_metric;

  a = a / 100.0;
  p = p / 100.0;
  z = invnorm(a);

  papi_init();

  /* This allocates papi_events */
  papi_get_events(papi_metric, &papi_events, &papi_event_total);
  papi_set_events(papi_events, 1);

  papi_data_init(&total, 1);

  /* Take measurement */
  papi_profile(so_init, so_call, so_free, &total, 1, z, p);

  /* Print out counter values */
  if (stats)
    printf("%lld %.2Lf %zd", total.mean[0], sqrtl((long double) total.stdev[0]),total.samples); 
  else
    printf("%lld", total.mean[0]);
  printf("\n");

  papi_data_free(&total);

  wht_free(W);
  free(x);

  return 0;
}
