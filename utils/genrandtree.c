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
  printf("wht_randtree: -n SIZE\n");
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

  while ((c = getopt (argc, argv, "hvn:")) != -1)
    switch (c) {
      case 'n':
        wht_size = atoi(optarg);
        break;
      case 'h':
        usage();
      default:
        usage();
    }

  if (wht_size == 0)
    usage();

  min_leaf = 1;
  max_leaf = 4;

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
