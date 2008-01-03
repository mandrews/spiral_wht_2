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

  labeled_vector& operator+=(labeled_vector &v);

  friend ostream& operator<<(ostream& o, labeled_vector &v);
};

typedef labeled_vector::iterator labeled_vector_iter;

#endif/*LABELED_VECTOR_H*/
