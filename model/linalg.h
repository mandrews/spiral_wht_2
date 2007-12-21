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

struct matrix * matrix_transpose(struct matrix *a);

struct qr_set* modified_gram_schmidt(struct matrix *a);

struct matrix * matrix_least_squares_error(struct matrix *a, struct matrix *b);

#define matrix_elem(A,i,j) (A->v[i + j*(A->m)])

void matrix_row_copy(struct matrix *a, size_t k, struct matrix *b, size_t l);
void matrix_col_copy(struct matrix *a, size_t k, struct matrix *b, size_t l);

#endif/*LINALG_H*/
