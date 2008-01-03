extern "C" {
#include "wht.h"
#include "linalg.h"
#include "getopt.h"
}

#include "labeled_vector.h"
#include "ic_count.h"

using std::cout;
using std::endl;

void 
interp_coeff(labeled_vector *basis, labeled_vector *count)
{
  labeled_vector *coeffs, *counts;
  labeled_vector_iter i;
  size_t k, m, max;
  struct matrix *a, *b, *c;
  char *tmp;
  Wht *W;

  m = basis->size();
  max = 4;

  a = matrix_init(m,m);
  b = matrix_init(m,1);

  i = count->begin();

  for (k = 0; i != count->end(); i++, k++) {
    tmp = (char *) i->first.c_str();

    W = wht_parse(tmp);
    counts = ic_count(W, basis, max);

    labeled_vector_to_matrix_row(counts, a, k);

    matrix_elem(b,k,0) = i->second;

    wht_free(W);
    delete counts;
  }

  coeffs = labeled_vector_copy(basis);

  c = matrix_least_squares_error(a,b);

  matrix_col_to_labeled_vector(c,0,coeffs);

  save_labeled_vector(stdout, coeffs);

  matrix_free(a);
  matrix_free(b);
  matrix_free(c);

  delete coeffs;
}


static void
usage() 
{
  printf("Usage: interp_coeff -b BASIS -s SAMPLE [OPTIONS]\n\n");
  printf("    -h        Show this help message.\n");
  printf("    -v        Show build information.\n");
  printf("    -b        Basis WHT plans.\n");
  printf("    -c        Counts for WHT plans.\n");
  exit(1);
}

int
main(int argc, char **argv)
{
  char *basis_file, *count_file;
  int c;

  basis_file   = NULL;
  count_file  = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hvb:c:")) != -1)
    switch (c) {
      case 'b':
        basis_file  = optarg;
        break;
      case 'c':
        count_file = optarg;
        break;
      case 'h':
        usage();
      case 'v':
        wht_info();
        exit(0);
      default:
        usage();
    }

  if (basis_file == NULL)
    usage();

  if (count_file == NULL)
    usage();

  FILE *fd;
  labeled_vector *basis, *count;

  fd = fopen(basis_file, "r");
  basis = load_labeled_vector(fd);
  fclose(fd);

  fd = fopen(count_file, "r");
  count = load_labeled_vector(fd);
  fclose(fd);

  interp_coeff(basis, count);

  delete basis;
  delete count;

  return 0;
}
