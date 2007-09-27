#include "tree.h"
#include "randtree.h"
#include "rand.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "getopt.h"

static void
usage() 
{
  printf("wht_randtree: -n SIZE [-a MIN_NODE -b MAX_NODE] \n");
  exit(1);
}

int min_leaf, max_leaf;

int
main(int argc, char **argv)
{
  size_t wht_size;
  int c;

  wht_size = 0;

  opterr = 0;

  min_leaf = 1;
  max_leaf = WHT_MAX_UNROLL;

  while ((c = getopt (argc, argv, "hvn:a:b:")) != -1)
    switch (c) {
      case 'n':
        wht_size = atoi(optarg);
        break;
      case 'a':
        min_leaf = atoi(optarg);
        break;
      case 'b':
        max_leaf = atoi(optarg);
        break;
      case 'h':
        usage();
      default:
        usage();
    }

  if (wht_size == 0)
    usage();

  if (min_leaf < 1) {
    printf("-a MIN_NODE too small.\n");
    exit(1);
  }

  if (max_leaf > WHT_MAX_UNROLL) {
    printf("-b MAX_NODE cannot be larger than %d.\n", WHT_MAX_UNROLL);
    exit(1);
  }

  node *root;
  char *buf;

  init_rand(getpid());

  //root = rec_uniform_splitting(wht_size);
  root = rec_series_splitting(wht_size);

  buf = tree_to_formula(root);
  printf("%s\n", buf);

  node_free(root);
  free(buf);

  return 0;
}
