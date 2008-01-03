extern "C" {
#include "wht.h"
#include "getopt.h"
}

#include <iostream>

#include "count.h"
#include "labeled_vector.h"

using std::cout;

static void
usage() 
{
  printf("Usage: wht_count -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w        WHT plan.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan;
  int c;

  plan = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:")) != -1)
    switch (c) {
      case 'w':
        plan = optarg;
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (plan == NULL)
    usage();

  Wht *W;
  labeled_vector *v;

  W = wht_parse(plan);

  v = count(W, 4);

  cout << (*v);

  wht_free(W);
  delete v;

  return 0;
}
