#ifndef LINALG_H
#define LINALG_H

struct matrix {
  double *v;
  size_t m;
  size_t n;
};

struct qr_set {
  struct matrix *q;
  struct matrix *r;
};

struct matrix * matrix_init(size_t m, size_t n);

void matrix_free(struct matrix *a);

void matrix_print(struct matrix *a);

void matrix_row_add(struct matrix *a, size_t k, double row[], size_t p);

void matrix_col_add(struct matrix *a, size_t k, double col[], size_t p);

struct matrix * matrix_transpose(struct matrix *a);

struct qr_set* modified_gram_schmidt(struct matrix *a);

struct matrix * matrix_least_squares_error(struct matrix *a, struct matrix *b);

#define matrix_elem(A,i,j) (A->v[i + j*(A->m)])

#endif/*LINALG_H*/
