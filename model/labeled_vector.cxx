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

labeled_vector *
load_labeled_vector(FILE *fd)
{
  char *buf;
  size_t max = 256;
  labeled_vector *basis;
  string line, plan, num;

  buf = (char *) malloc(sizeof(char) * max);

  basis = new labeled_vector();

  while (getline(&buf, &max, fd) > 0) {
    line = string(buf);
    line = line.substr(0, line.find("\n")); /* Chomp! */

    if (line.empty())
      continue;

    if (line.find(" : ") != string::npos) {
      plan = line.substr(0, line.find(" : "));
      num  = line.substr(line.find(" : ") + 3, line.length());

      (*basis)[plan] = atof(num.c_str());
    } else {
      (*basis)[line] = 0.0;
    }
  }

  free(buf);

  return basis;
}

void
save_labeled_vector(FILE *fd, labeled_vector *v)
{
  labeled_vector::iterator i;

  for (i = v->begin(); i != v->end(); i++) {
    fprintf(fd, "%s : %f\n", i->first.c_str(), i->second);
  }
}


labeled_vector *
labeled_vector_copy(labeled_vector *src)
{
  labeled_vector *dst;
  labeled_vector::iterator i;

  dst = new labeled_vector();

  for (i = src->begin(); i != src->end(); i++)
    (*dst)[i->first] = i->second;

  return dst;
}