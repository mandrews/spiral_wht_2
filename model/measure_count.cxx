extern "C" {
#include "wht.h"
#include "measure.h"
#include "linalg.h"
#include "getopt.h"
#include "store.h"
}

#include "labeled_vector.h"

static void
usage() 
{
  printf("Usage: wht_measure_count -b BASIS -s SAMPLE [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -b        Basis WHT plans.\n");
  printf("    -s        Sampling WHT plans.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *basis, *sample;
  int c;

  basis   = NULL;
  sample  = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvb:s:")) != -1)
    switch (c) {
      case 'b':
        basis  = optarg;
        break;
      case 's':
        sample = optarg;
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (basis == NULL)
    usage();

  if (sample == NULL)
    usage();

  return 0;
}
