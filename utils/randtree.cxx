extern "C" {
#include "wht.h"
}

#include "combin.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>

#include "getopt.h"

static void
usage() 
{
  printf("Usage: wht_randtree -n SIZE [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -n SIZE   Total size of random composition tree.\n");
  printf("    -a MIN    Minimum number of factors in composition.\n");
  printf("    -b MAX    Maximum number of factors in composition.\n");
  printf("    -p MIN    Minimum element in composition.\n");
  printf("    -q MAX    Maximum element in composition.\n");
  printf("    -t TYPE   Type of WHT to generate (DEFAULT \"general\").\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  size_t wht_size, min_child, max_child, min_leaf, max_leaf;
  char *wht_type;

  int c;

  wht_size = 0;

  opterr = 0;

	min_child = 1;
	max_child = MAX_SPLIT_NODES;
  min_leaf 	= 1;
  max_leaf 	= WHT_MAX_UNROLL;
  wht_type  = NULL;

  while ((c = getopt (argc, argv, "hvn:a:b:p:q:t:")) != -1)
    switch (c) {
      case 'n':
        wht_size = atoi(optarg);
        break;
      case 'a':
        min_child = atoi(optarg);
        break;
      case 'b':
        max_child = atoi(optarg);
        break;
      case 'p':
        min_leaf = atoi(optarg);
        break;
      case 'q':
        max_leaf = atoi(optarg);
        break;
      case 't':
        wht_type = optarg;
        break;
      case 'h':
        usage();
			case 'v':
				wht_info();
				exit(0);
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

  if (wht_type != NULL && (strcmp(wht_type, "general" ) != 0) 
                       && (strcmp(wht_type, "right"   ) != 0)) {
    printf("-t TYPE can be \"general\" or \"right\".\n");
    exit(1);
  }

  compos_node *root;
  char *buf;

  srandom((unsigned int) (getpid() * M_PI));

  root = NULL;

  if (wht_type == NULL || (strcmp(wht_type, "general") == 0))
	  root = compos_tree_rand(wht_size, min_child, max_child, min_leaf, max_leaf);
  else if (strcmp(wht_type, "right") == 0)
    root = compos_tree_rand_right(wht_size, min_child, max_child, min_leaf, max_leaf);

  buf = compos_tree_to_string(root);
  printf("%s\n", buf);

  compos_tree_free(root);

  free(buf);

  return 0;
}
