#include "labeled_vector.h"

labeled_real::labeled_real(string s, double d)
{
  this->first = s;
  this->second = d;
}

labeled_real& 
labeled_real::operator=(double d)
{
  this->second = d;
  return *this;
}

labeled_real& 
labeled_real::operator+=(double d)
{
  this->second += d;
  return *this;
}

labeled_real& 
labeled_real::operator+=(labeled_real &r)
{
  this->second += r.second;
  return *this;
}


ostream&
operator<<(ostream& o, labeled_real &b) 
{
  return o << b.second;
}

labeled_real &
labeled_vector::operator[](string s) 
{
  labeled_vector_iter i;

  for (i = this->begin(); i != this->end(); i++)
    if (i->first == s)
      return *i;

  this->push_back(labeled_real(s,0.0));

  return this->back();
}

ostream&
operator<<(ostream& o, labeled_vector &v) 
{
  labeled_vector_iter i;

  for (i = v.begin(); i != v.end(); i++)
    o << i->first << " : "<< i->second << std::endl;

  return o;
}

labeled_vector &
labeled_vector::operator+=(labeled_vector &v)
{
  labeled_vector_iter i;

  for (i = v.begin(); i != v.end(); i++)
    (*this)[i->first] += v[i->first];
}

