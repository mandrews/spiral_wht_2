#ifndef LABELED_VECTOR_H
#define LABELED_VECTOR_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

using std::vector;
using std::pair;
using std::string;
using std::ostream;

class labeled_real: public pair<string, double>
{
  public:
  labeled_real(string s, double d);

  labeled_real& operator=(double d);

  labeled_real& operator+=(double d);
  labeled_real& operator+=(labeled_real &r);

  friend double operator*(labeled_real &r, double d) {
    return r.second * d;
  }

  friend double operator*(double d, labeled_real &r) {
    return r.second * d;
  }

  friend ostream& operator<<(ostream& o, labeled_real &b);
};

class labeled_vector : public vector<labeled_real>
{
  public:
  labeled_real & operator[](string s);

  friend ostream& operator<<(ostream& o, labeled_vector &v);
};

typedef labeled_vector::iterator labeled_vector_iter;

void labeled_vector_add(labeled_vector *into, labeled_vector *from);

void matrix_row_to_labeled_vector(struct matrix *a, size_t k, labeled_vector *r);
void matrix_col_to_labeled_vector(struct matrix *a, size_t k, labeled_vector *r);

void labeled_vector_to_matrix_row(labeled_vector *r, struct matrix *a, size_t k);
void labeled_vector_to_matrix_col(labeled_vector *r, struct matrix *a, size_t k);

void labeled_vector_set(labeled_vector *v, string key, double value);
void labeled_vector_add(labeled_vector *v, string key, double value);

labeled_vector * load_labeled_vector(FILE *fd);
void save_labeled_vector(FILE *fd, labeled_vector *v);

labeled_vector * labeled_vector_copy(labeled_vector *src);

#endif/*LABELED_VECTOR_H*/
