extern "C" {
#include "wht.h"
#include "getopt.h"
}

#include "count.h"
#include <stdio.h>

static void
usage() 
{
  printf("Usage: wht_predict -f COEFF [-c COUNT] [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *count_file;
  char *coeff_file;
  int c;

  count_file = NULL;
  coeff_file = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvc:f:")) != -1)
    switch (c) {
      case 'c':
        count_file = optarg;
        break;
      case 'f':
        coeff_file = optarg;
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (coeff_file == NULL)
    usage();

  FILE *fd;
  counts x, y;
  counts_iter i;
  double sum;

  if (count_file != NULL) {
    fd = fopen(count_file, "r");
    x.load_counts(fd);
    fclose(fd);
  } else {
    x.load_counts(stdin);
  }

  fd = fopen(coeff_file, "r");
  y.load_counts(fd);
  fclose(fd);

  x *= y;

  sum = 0;
  for (i = x.begin(); i != x.end(); i++)
    sum += i->second;

  printf("%f\n", sum);

  return 0;
}
