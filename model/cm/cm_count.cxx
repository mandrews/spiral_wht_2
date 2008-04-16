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
  printf("    -c CAPACITY   Capacity of cache (in bytes).\n");
  printf("    -b BLOCK      Size of cache block (in bytes).\n");
  printf("    -a ASSOC      Set associativity of cache.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *plan;
  long C, b, a;
  bool overhead;
  size_t len;
  int c;

  plan = NULL;
  C = -1;
  b = 1;
  a = 1;
  overhead = false;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvw:c:b:a:o")) != -1)
    switch (c) {
      case 'w':
        plan = strdup(optarg);
        break;
      case 'c':
        C = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 'a':
        a = atoi(optarg);
        break;
      case 'o':
        overhead = true;
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

  if (! i_power_of_2(b)) {
    printf("Option -b BLOCK needs to be a power of 2\n");
    exit(1);
  }

  if (! i_power_of_2(a)) {
    printf("Option -a ASSOC needs to be a power of 2\n");
    exit(1);
  }

  if (plan == NULL)
    getline(&plan, &len, stdin);

  if (plan == NULL)
    usage();


  Wht *W;
  long cm, ov;

  C /= sizeof(wht_value);
  b /= sizeof(wht_value);
  // C /= a;
  // a = (long) ceil(log(a) / log(2));

  W = wht_parse(plan);

  if (wht_error_msg(W) != NULL) {
    printf("rejected, %s\n", wht_error_msg(W));
    free(plan);
    wht_free(W);
    exit(1);
  }

  ov = (W->N / b)*(a-1);
  cm = cm_count(W, 1, C, b, a);
  
  if (overhead)
    /* Prefetch factor */
    printf("%ld\n", (long) (ceil((cm-ov)/(4.0 - 1))));
  else
    printf("%ld\n", cm);

  wht_free(W);

  free(plan);

  return 0;
}

