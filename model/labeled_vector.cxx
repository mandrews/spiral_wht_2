extern "C" {
#include "linalg.h"
}

#include <string>
#include <map>

using std::string;
using std::map;

typedef map<const string, double> labeled_vector;
typedef map<const string, double>::iterator labeled_vector_iter;

void
labeled_vector_add(labeled_vector *into, labeled_vector *from)
{
  labeled_vector_iter i;

  for (i = from->begin(); i != from->end(); i++)
    (*into)[i->first] += i->second;
}

void
matrix_row_to_labeled_vector(struct matrix *a, size_t k, labeled_vector *r)
{
  size_t n, j;
  labeled_vector_iter i;

  n = a->n;

  assert(k <= n);

  for (j = 0, i = r->begin(); j < n && i != r->end(); j++, i++) 
    i->second = matrix_elem(a,k,j);
}

void
matrix_col_to_labeled_vector(struct matrix *a, size_t k, labeled_vector *r)
{
  size_t m, j;
  labeled_vector_iter i;

  m = a->m;

  assert(k <= m);

  for (j = 0, i = r->begin(); j < m && i != r->end(); j++, i++) 
    i->second = matrix_elem(a,j,k);
}

void
labeled_vector_to_matrix_row(labeled_vector *r, struct matrix *a, size_t k)
{
  size_t n, j;
  labeled_vector_iter i;

  n = a->n;

  assert(k <= n);

  for (j = 0, i = r->begin(); j < n && i != r->end(); j++, i++) 
    matrix_elem(a,k,j) = i->second;
}

void
labeled_vector_to_matrix_col(labeled_vector *r, struct matrix *a, size_t k)
{
  size_t m, j;
  labeled_vector_iter i;

  m = a->m;

  assert(k <= m);

  for (j = 0, i = r->begin(); j < m && i != r->end(); j++, i++) 
    matrix_elem(a,j,k) = i->second;
}

