#ifndef LABELED_VECTOR_H
#define LABELED_VECTOR_H

#include <string>
#include <map>

using std::string;
using std::map;

typedef map<const string, double> labeled_vector;
typedef map<const string, double>::iterator labeled_vector_iter;

void labeled_vector_add(labeled_vector *into, labeled_vector *from);

void matrix_row_to_labeled_vector(struct matrix *a, size_t k, labeled_vector *r);
void matrix_col_to_labeled_vector(struct matrix *a, size_t k, labeled_vector *r);

void labeled_vector_to_matrix_row(labeled_vector *r, struct matrix *a, size_t k);
void labeled_vector_to_matrix_col(labeled_vector *r, struct matrix *a, size_t k);

labeled_vector * load_labeled_vector(FILE *fd);
void save_labeled_vector(FILE *fd, labeled_vector *v);

labeled_vector * labeled_vector_copy(labeled_vector *src);

#endif/*LABELED_VECTOR_H*/
