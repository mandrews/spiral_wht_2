#include "count.h"

string 
small_to_key(Wht *W)
{
  char *buf;
  string key;

  buf = W->to_string(W);
  key = buf;
  free(buf);
  return key;
}

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

  key.append(W->name);
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
    key = small_to_key(W);

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
#if 1
    if (Wi->attr[interleave_by] != UNSET_PARAMETER)
      il = Wi->attr[interleave_by];
#endif

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

  key.append(W->name);
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

  key.append(W->name);
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

  key.append(W->name);
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

counts *
default_basis(size_t max)
{
  counts *basis;
  basis = new counts();
#if 0
  size_t i;

  for (i = 1; i <= max; i++)
    basis[string("small[") + string(i) + string("]")] = 0.0;
#endif

  (*basis)["small[1]"] = 0.0;
  (*basis)["small[2]"] = 0.0;
  (*basis)["small[3]"] = 0.0;
  (*basis)["small[4]"] = 0.0;
  (*basis)["small[5]"] = 0.0;
  (*basis)["small[6]"] = 0.0;
  (*basis)["small[7]"] = 0.0;
  (*basis)["small[8]"] = 0.0;
  (*basis)["split_alpha"] = 0.0;
  (*basis)["split_beta_1"] = 0.0;
  (*basis)["split_beta_2"] = 0.0;
  (*basis)["split_beta_3"] = 0.0;

  return basis;
}

counts &
counts::operator+=(counts &x)
{
  counts_iter i;

  for (i = x.begin(); i != x.end(); i++)
    (*this)[i->first] += x[i->first];

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


counts *
count(Wht *W, size_t max)
{
  size_t k;
  counts *x, *t;

  x = default_basis(max);

  t = alpha_n(W);
  *x += *t;
  delete t;

  t = beta_1(W);
  *x += *t;
  delete t;

  t = beta_2(W);
  *x += *t;
  delete t;

  t = beta_3(W);
  *x += *t;
  delete t;

  for (k = 1; k <= max; k++) {
    t = alpha_k(W,k);
    *x += *t;
    delete t;
  }

  return x;
}

