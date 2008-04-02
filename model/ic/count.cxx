#include "count.h"

#include <sstream>
#include <string>

/**
 * \todo replace this type of signature with a more idiomatic c++ signature,
 * e.g.
 *   void alpha_n(Wht *W, counts& x)
 */
counts *
alpha_n(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nn, i;
  counts *x, *t;
  string key;

  counts_iter j;

  x = new counts();

  if (W->children == NULL)
    return x;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->rule->name);
  key.append("_alpha");

  (*x)[key] = 1;

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    t = alpha_n(Wi);

    for (j = t->begin(); j != t->end(); j++) 
      (*x)[j->first] += ((1 << (n - ni)) * (*t)[j->first]);

    delete t;
  }

  return x;
}

counts *
alpha_k(Wht *W, size_t k)
{
  Wht *Wi;
  size_t n, ni, nn;
  int i;
  counts *x, *t;
  string key;

  counts_iter j;

  x = new counts();

  if (W->children == NULL) {
    key = string(W->to_string);

    if (W->n == (int) k)
      (*x)[key] = 1;
    else
      (*x)[key] = 0;

    return x;
  }

  n  = W->n;
  nn = W->children->nn;

  for (i = nn - 1; i >= 0; i--) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    t = alpha_k(Wi,k);

    int il = 1;
    if (Wi->attr[INTERLEAVE_BY] != UNSET_PARAMETER)
      il = Wi->attr[INTERLEAVE_BY];

    for (j = t->begin(); j != t->end(); j++) 
      (*x)[j->first] += (((1 << (n - ni)) * j->second) / il);

    delete t;
  }

  return x;
}

counts *
beta_1(Wht *W)
{

  Wht *Wi;
  size_t n, ni, nn;
  int i;
  counts *x, *t;
  string key;

  counts_iter j;

  x = new counts();

  if (W->children == NULL)
    return x;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->rule->name);
  key.append("_beta_1");

  (*x)[key] = nn;

  for (i = nn - 1; i >= 0; i--) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    t = beta_1(Wi);

    for (j = t->begin(); j != t->end(); j++) 
      (*x)[j->first] += ((1 << (n - ni)) * (*t)[j->first]);

    delete t;
  }

  return x;
}

counts *
beta_2(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nj, nn;
  int i;
  counts *x, *t;
  string key;

  counts_iter j;

  x = new counts();

  if (W->children == NULL)
    return x;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->rule->name);
  key.append("_beta_2");

  (*x)[key] = 0;

  for (i = nn - 1, nj = 0; i >= 0; i--, nj += ni) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    t = beta_2(Wi);

    for (j = t->begin(); j != t->end(); j++) 
      (*x)[j->first] += ((1 << (n - ni)) * (*t)[j->first]);

    (*x)[key] +=  (1 << nj);

    delete t;
  }

  return x;
}

counts *
beta_3(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nn;
  int i;
  counts *x, *t;
  string key;

  counts_iter j;

  x = new counts();

  if (W->children == NULL)
    return x;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->rule->name);
  key.append("_beta_3");

  (*x)[key] = 0;

  for (i = nn - 1; i >= 0; i--) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    t = beta_3(Wi);

    for (j = t->begin(); j != t->end(); j++) 
      (*x)[j->first] += ((1 << (n - ni)) * (*t)[j->first]);

    (*x)[key] +=  (1 << (n - ni));

    delete t;
  }

  return x;
}

counts::counts(size_t max)
{
  size_t i;

  for (i = 1; i <= max; i++) {
    std::ostringstream key;
    key  << "small[" << i << "]";
    (*this)[key.str()] = 0.0;
  }

  (*this)["split_alpha"] = 0.0;
  (*this)["split_beta_1"] = 0.0;
  (*this)["split_beta_2"] = 0.0;
  (*this)["split_beta_3"] = 0.0;

  this->max = max;
}

counts &
counts::operator+=(counts &x)
{
  counts_iter i;

  for (i = x.begin(); i != x.end(); i++)
    (*this)[i->first] += x[i->first];

  return (*this);
}

counts &
counts::operator*=(counts &x)
{
  counts_iter i;

  for (i = x.begin(); i != x.end(); i++)
    (*this)[i->first] *= x[i->first];

  return (*this);
}

ostream&
operator<<(ostream& o, counts &x) 
{
  counts_iter i;

  for (i = x.begin(); i != x.end(); i++)
    o << i->first << " : "<< i->second << std::endl;

  return o;
}

void
counts::load_counts(FILE *fd)
{
  char *buf;
  size_t max = 256;
  string line, plan, num;

  buf = (char *) malloc(sizeof(char) * max);

  /** \todo replace with STL string::getline */
  while (getline(&buf, &max, fd) > 0) {
    line = string(buf);
    line = line.substr(0, line.find("\n")); /* Chomp! */

    if (line.empty())
      continue;

    if (line.find(" : ") != string::npos) {
      plan = line.substr(0, line.find(" : "));
      num  = line.substr(line.find(" : ") + 3, line.length());

      (*this)[plan] = atof(num.c_str());
    } else {
      (*this)[line] = 0.0;
    }
  }

  free(buf);
}

void
counts::load_basis(FILE *fd)
{
  counts_iter i;

  load_counts(fd);

  for (i = this->begin(); i != this->end(); i++)
    i->second = 0.0;
}

void
count(Wht *W, counts &x)
{
  size_t k;
  counts *t;

  t = alpha_n(W);
  x += *t;
  delete t;

  t = beta_1(W);
  x += *t;
  delete t;

  t = beta_2(W);
  x += *t;
  delete t;

  t = beta_3(W);
  x += *t;
  delete t;

  for (k = 1; k <= x.max; k++) {
    t = alpha_k(W,k);
    x += *t;
    delete t;
  }
}

