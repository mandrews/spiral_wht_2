#include "wht.h"

#include <sys/types.h>
#include <sys/resource.h>
#include <getopt.h>

double
cputime()
{
  struct rusage rus;

  getrusage (RUSAGE_SELF, &rus);
  return ((double) rus.ru_utime.tv_sec) * 1e6 + ((double) rus.ru_utime.tv_usec);
}


static void
usage() 
{
  printf("Usage: wht_measure -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w PLAN   Measure the average cost (in microseconds) to execute PLAN.\n");
  printf("    -t TIME   Accumulate average for TIME seconds.\n");
  exit(1);
}

int
main (int argc, char **argv)
{
  double max_time;
  char *wht_plan;
  int c;

  wht_plan = NULL;
  max_time = 1e5;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:t:")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
        break;
      case 't':
        max_time = atof(optarg) * 1e6;
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

  Wht *W;
  wht_value *x;
  long n,N;
  int i, repeats;

  double t0, avg_time;

  W = wht_parse(wht_plan);

  n = W->n;
  N = W->N;

  x = wht_random_vector(N);

  t0 = 0;

  avg_time = 0.0;
  repeats  = 1;

  while (avg_time < max_time) {
    t0 = cputime();
    for (i = 0; i < repeats; i++) 
      wht_apply(W,x);

    avg_time = cputime() - t0;
    repeats *= 2;
  }
  repeats /= 2;


  printf("%e\n",(avg_time/repeats));


  wht_free(W);
  free(x);

  return 0;
}
