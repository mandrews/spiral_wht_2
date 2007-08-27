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
  printf("measure: -w PLAN\n");
  exit(1);
}

int
main (int argc, char **argv)
{
  char *wht_plan;
  int c;

  wht_plan = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hw:")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
        break;
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

  double t0, max_time, avg_time;

  W = wht_parse(wht_plan);

  n = W->n;
  N = W->N;

  x = wht_random(N);

  t0 = 0;

  max_time = 2.0;
  avg_time = 0.0;
  repeats  = 1;

  while (avg_time < max_time) {
    t0 = cputime();
    for (i = 0; i < repeats; i++) 
      wht_apply(W,1,0,x);

    avg_time = cputime() - t0;
    repeats *= 2;
  }
  repeats /= 2;


  printf("%e\n",(avg_time/repeats));

  return 0;
}
