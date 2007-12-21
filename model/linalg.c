#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "linalg.h"

struct matrix *
matrix_init(size_t m, size_t n)
{
  struct matrix *a;
  int i,j;

  a = malloc(sizeof(*a));
  a->m = m;
  a->n = n;
  a->v = malloc(sizeof(*a->v) * n * m);

  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      matrix_elem(a,i,j) = 0.0;

  return a;
}

void 
matrix_free(struct matrix *a)
{
  free(a->v);
  free(a);
}

void
matrix_print(struct matrix *a)
{
  size_t m,n;
  int i,j;

  m = a->m;
  n = a->n;

  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      printf("% .5f ", matrix_elem(a,i,j));
    }
    printf("\n");
  }
}

/* Copy a_k to b_l */
void
matrix_col_copy(struct matrix *a, size_t k, struct matrix *b, size_t l)
{
  size_t m;
  int i;

  assert(a->m == b->m);
  assert(k >= 0);
  assert(k < a->n);
  assert(l >= 0);
  assert(l < b->n);

  m = a->m;

  for (i = 0; i < m; i++)
    matrix_elem(b,i,l) = matrix_elem(a,i,k);
}

void
matrix_row_copy(struct matrix *a, size_t k, struct matrix *b, size_t l)
{
  size_t n;
  int j;

  assert(a->n == b->n);

  n = a->n;

  for (j = 0; j < n; j++)
    matrix_elem(b,l,j) = matrix_elem(a,k,j);
}

struct matrix *
matrix_transpose(struct matrix *a)
{
  size_t m,n;
  int i, j;
  struct matrix *b;

  m = a->m;
  n = a->n;
  b = matrix_init(n,m);

  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) 
      matrix_elem(b,j,i) = matrix_elem(a,i,j);

  return b;
}

/* Compute a_k* * b_l or <a_k,b_l> */
double
matrix_col_iprod(struct matrix *a, size_t k, struct matrix *b, size_t l)
{
  size_t m,n;
  int i;
  double sum;

  assert(a->n == b->m);
  assert(k >= 0);
  assert(k < a->m);
  assert(l >= 0);
  assert(l < b->n);

  m = a->m;
  n = a->n;

  sum = 0.0;
  for (i = 0; i < n; i++)
    sum += matrix_elem(a,k,i) * matrix_elem(b,i,l);
  return sum;
}

/* Compute a_k* * b_l or <a_k,b_l> */
double
matrix_col_iprod_transpose(struct matrix *a, size_t k, struct matrix *b, size_t l)
{
  size_t m,n;
  int i;
  double sum;

  assert(k >= 0);
  assert(k < a->n);
  assert(l >= 0);
  assert(l < b->n);

  m = a->m;
  n = a->n;

  sum = 0.0;
  for (i = 0; i < n; i++)
    sum += matrix_elem(a,i,k) * matrix_elem(b,i,l);
  return sum;
}


void
matrix_col_add_vector(struct matrix *a, size_t k, struct matrix *b, size_t l)
{
  size_t m;
  int i;

  assert(a->m == b->m);
  assert(k >= 0);
  assert(k < a->n);
  assert(l >= 0);
  assert(l < b->n);

  m = a->m;

  for (i = 0; i < m; i++)
    matrix_elem(a,i,k) += matrix_elem(b,i,l);
}

void
matrix_col_mul_scalar(struct matrix *a, size_t k, double r)
{
  size_t m;
  int i;

  assert(k >= 0);
  assert(k < a->n);

  m = a->m;

  for (i = 0; i < m; i++)
    matrix_elem(a,i,k) *= r;
}

void
matrix_col_div_scalar(struct matrix *a, size_t k, double r)
{
  size_t m;
  int i;

  assert(r != 0.0);
  assert(k >= 0);
  assert(k < a->n);

  m = a->m;

  for (i = 0; i < m; i++)
    matrix_elem(a,i,k) /= r;
}

double
matrix_col_norm(struct matrix *a, size_t k)
{
  return sqrt(matrix_col_iprod_transpose(a,k,a,k));
}

/**
 * Derived from Algorithm 8.1 (p. 58)
 *
 * \see 
 * @book{
 *  author = {Trefethen, Lloyd  N.  and Bau, David  },
 *  isbn = {0898713617},
 *  publisher = {{SIAM: Society for Industrial and Applied Mathematics}},
 *  title = {Numerical Linear Algebra},
 *  month = {June},
 *  year = {1997}
 * }
 */
struct qr_set*
modified_gram_schmidt(struct matrix *a)
{
  size_t m,n;
  int i,j;
  double rii, rij;
  struct matrix *v, *q, *r, *qi;
  struct qr_set *set;

  assert(a->m >= a->n);

  m = a->m;
  n = a->n;
  v = matrix_init(m,n);
  r = matrix_init(m,n);
  q = matrix_init(m,m);

  qi = matrix_init(m,1);

  set = malloc(sizeof(*set));

  for (i = 0; i < n; i++)
    matrix_col_copy(a,i,v,i);

  for (i = 0; i < n; i++) {
    rii = matrix_col_norm(v,i);

    matrix_elem(r,i,i) = rii;
    matrix_col_copy(v,i,q,i);
    matrix_col_div_scalar(q,i,rii);

    for (j = i+1; j < n; j++) {

      rij = matrix_col_iprod_transpose(q,i,v,j);

      matrix_elem(r,i,j) = rij;

      matrix_col_copy(q,i,qi,0);
      matrix_col_mul_scalar(qi, 0, -1.0*rij);
      matrix_col_add_vector(v, j, qi, 0);
    }
  }

  matrix_free(v);
  matrix_free(qi);

  set->q = q;
  set->r = r;

  return set;
}

/* c = a * b */
struct matrix *
matrix_mul(struct matrix *a, struct matrix *b)
{
  assert(a->n == b->m);

  size_t m,n;
  int i, j;
  struct matrix *c;

  m = a->m;
  n = b->n;
  c = matrix_init(m,n);

  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) 
      matrix_elem(c,i,j) = matrix_col_iprod(a,i,b,j);

  return c;
}

/**
 * Derived from Algorithm 17.1 (p. 122)
 *
 * \see 
 * @book{
 *  author = {Trefethen, Lloyd  N.  and Bau, David  },
 *  isbn = {0898713617},
 *  publisher = {{SIAM: Society for Industrial and Applied Mathematics}},
 *  title = {Numerical Linear Algebra},
 *  month = {June},
 *  year = {1997}
 * }
 */
struct matrix *
matrix_backsubs(struct matrix *a, struct matrix *b)
{
  assert(a->n == b->m);
  assert(b->n == 1);

  int j,k;
  size_t m;
  double sum;
  struct matrix *x;

  m = a->n;
  x = matrix_init(m,1);

  for (j = m-1; j >= 0; j--) {
    sum = 0.0;
    for (k = j+1; k < m; k++) {
      sum += matrix_elem(x,k,0) * matrix_elem(a,j,k);
    }
    matrix_elem(x,j,0) = (matrix_elem(b,j,0) - sum) / matrix_elem(a,j,j);
  }

  return x;
}

/**
 * Derived from Algorithm 12.2 (p. 83)
 *
 * \see 
 * @book{
 *  author = {Trefethen, Lloyd  N.  and Bau, David  },
 *  isbn = {0898713617},
 *  publisher = {{SIAM: Society for Industrial and Applied Mathematics}},
 *  title = {Numerical Linear Algebra},
 *  month = {June},
 *  year = {1997}
 * }
 */
struct matrix *
matrix_least_squares_error(struct matrix *a, struct matrix *b)
{
  assert(a->m == b->m);
  assert(b->n == 1);

  struct qr_set *qr;
  struct matrix *bp, *qt, *x;

  qr = modified_gram_schmidt(a);

  qt = matrix_transpose(qr->q);

  bp = matrix_mul(qt, b);

  x = matrix_backsubs(qr->r, bp);

  matrix_free(qr->q);
  matrix_free(qr->r);
  matrix_free(qt);
  matrix_free(bp);

  free(qr);

  return x;
}

#if 0
int 
main()
{
  struct matrix *a, *b, *c, *d;

  a = matrix_init(5,3);

  matrix_elem(a,0,0) = 1;
  matrix_elem(a,0,1) = 2;
  matrix_elem(a,0,2) = 3;
  matrix_elem(a,1,0) = 4;
  matrix_elem(a,1,1) = 5;
  matrix_elem(a,1,2) = 6;
  matrix_elem(a,2,0) = 7;
  matrix_elem(a,2,1) = 8;
  matrix_elem(a,2,2) = 9;
  matrix_elem(a,3,0) = 10;
  matrix_elem(a,3,1) = 11;
  matrix_elem(a,3,2) = 12;
  matrix_elem(a,4,0) = 13;
  matrix_elem(a,4,1) = 14;
  matrix_elem(a,4,2) = 15;

  b = matrix_transpose(a);
  c = matrix_mul(a,b);

  printf("a:\n");
  matrix_print(a);

  printf("b:\n");
  matrix_print(b);

  printf("c:\n");
  matrix_print(c);

  struct qr_set *qr;

  qr = mgram_schmidt(a);

  printf("q:\n");
  matrix_print(qr->q);
  printf("r:\n");
  matrix_print(qr->r);

  d = matrix_mul(qr->q, qr->r);
  printf("qr:\n");
  matrix_print(d);

  matrix_free(qr->q);
  matrix_free(qr->r);
  free(qr);

  matrix_free(a);
  matrix_free(b);
  matrix_free(c);

  matrix_free(d);


  return 0;
}

int 
main()
{
  struct matrix *a, *b, *x;

  a = matrix_init(8,8);
  b = matrix_init(8,1);

  double a0[] = {0,    0,    0,    0,    1,    0,    0,    0};
  double a1[] = {0,    0,    0,    0,    0,    1,    0,    0};
  double a2[] = {0,    0,    0,    0,    0,    0,    1,    0};
  double a3[] = {0,    0,    0,    0,    0,    0,    0,    1};
  double a4[] = {1,    3,    7,   12,   12,    0,    0,    0};
  double a5[] = {9,   11,   15,   20,   12,    0,    0,    0};
  double a6[] = {3,    6,    9,   14,   12,    0,    0,    0};
  double a7[] = {3,    6,   11,   14,   12,    0,    0,    0};

  double b0[] = {8, 25, 65, 185, 466, 1034, 626, 644};

  matrix_row_add(a,0,a0,8);
  matrix_row_add(a,1,a1,8);
  matrix_row_add(a,2,a2,8);
  matrix_row_add(a,3,a3,8);
  matrix_row_add(a,4,a4,8);
  matrix_row_add(a,5,a5,8);
  matrix_row_add(a,6,a6,8);
  matrix_row_add(a,7,a7,8);

  matrix_col_add(b,0,b0,8);

  x = matrix_lse(a,b);

  printf("a:\n");
  matrix_print(a);
  printf("b:\n");
  matrix_print(b);
  printf("x:\n");
  matrix_print(x);

  matrix_free(a);
  matrix_free(b);
  matrix_free(x);

  return 0;
}
#endif
