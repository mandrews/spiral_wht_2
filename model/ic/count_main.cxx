extern "C" {
#include "wht.h"
#include "getopt.h"
}

#include <iostream>

#include "count.h"

using std::cout;

static void
usage() 
{
  printf("Usage: wht_count -w PLAN [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -w        WHT plan.\n");
  printf("    -b        Basis, i.e. extra codelets to count.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan;
  char *basis_file;
  int c;

  plan = NULL;
  basis_file = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:b:")) != -1)
    switch (c) {
      case 'w':
        plan = optarg;
        break;
      case 'b':
        basis_file = optarg;
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
  counts x;
  FILE *fd;

  W = wht_parse(plan);

  if (basis_file != NULL) {
    fd = fopen(basis_file, "r");
    x.load_basis(fd);
    fclose(fd);
  }

  count(W, x);

  cout << x;

  wht_free(W);

  return 0;
}
