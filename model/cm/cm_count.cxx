extern "C" {
#include "wht.h"
#include "getopt.h"
}

#include "model.h"

static void
usage() 
{
  printf("Usage: wht_verify -w PLAN [OPTIONS]\n");
  printf("Verify PLAN from stdin or by argument.\n");
  printf("    -h            Show this help message.\n");
  printf("    -v            Show build information.\n");
  printf("    -w PLAN       Verify correctness of PLAN.\n");
  printf("    -c CAPACITY   Capacity of cache in bytes.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan;
  size_t len;
  int c;
  long C;

  plan = NULL;
  C = -1;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:c:")) != -1)
    switch (c) {
      case 'w':
        plan = strdup(optarg);
        break;
      case 'c':
        C = atoi(optarg);
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (C < 0)
    usage();

  if (plan == NULL)
    getline(&plan, &len, stdin);

  if (plan == NULL)
    usage();


  Wht *W;
  long cm;

  W = wht_parse(plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    free(plan);
    wht_free(W);
    exit(1);
  }

  cm = cm_count(W, 1, C);

  printf("%ld\n", cm);

  wht_free(W);

  free(plan);

  return 0;
}

