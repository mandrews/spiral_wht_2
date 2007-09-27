#include "wht.h"
#include "papi_utils.h"

#include <getopt.h>
#include <stdbool.h>

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


static void
usage() 
{
  printf("Usage: wht_measure -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w PLAN   Measure the average cost (in cycles) to execute PLAN.\n");
  printf("    -s        Also measure the standard deviation and sample size.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *wht_plan;
  int c;
  bool stats;

  wht_plan = NULL;
  stats = false;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:s")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
        break;
      case 's':
        stats = true;
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

  papi_init();

  /* This allocates papi_events */
  papi_get_events("TOT_CYC", &papi_events, &papi_event_total);
  papi_set_events(papi_events, 1);

  papi_data_init(&total, 1);

  /* Take measurement */
  papi_profile(so_init, so_call, so_free, &total, 1);

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
