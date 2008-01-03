#include "count.h"
#include "labeled_vector.h"

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

labeled_vector *
alpha_n(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nn, i;
  labeled_vector *counts, *tmp_counts;
  string key;

  labeled_vector_iter j;

  counts = new labeled_vector();

  if (W->children == NULL)
    return counts;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->name);
  key.append("_alpha");

  (*counts)[key] = 1;

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = alpha_n(Wi);

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

    delete tmp_counts;
  }

  return counts;
}

labeled_vector *
alpha_k(Wht *W, size_t k)
{
  Wht *Wi;
  size_t n, ni, nn;
  int i;
  labeled_vector *counts, *tmp_counts;
  string key;

  labeled_vector_iter j;

  counts = new labeled_vector();

  if (W->children == NULL) {
    key = small_to_key(W);

    if (W->n == (int) k)
      (*counts)[key] = 1;
    else
      (*counts)[key] = 0;

    return counts;
  }

  n  = W->n;
  nn = W->children->nn;

  for (i = nn - 1; i >= 0; i--) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = alpha_k(Wi,k);

    int il = 1;
#if 1
    if (Wi->attr[interleave_by] != UNSET_PARAMETER)
      il = Wi->attr[interleave_by];
#endif

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += (((1 << (n - ni)) * j->second) / il);

    delete tmp_counts;
  }

  return counts;
}

labeled_vector *
beta_1(Wht *W)
{

  Wht *Wi;
  size_t n, ni, nn;
  int i;
  labeled_vector *counts, *tmp_counts;
  string key;

  labeled_vector_iter j;

  counts = new labeled_vector();

  if (W->children == NULL)
    return counts;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->name);
  key.append("_beta_1");

  (*counts)[key] = nn;

  for (i = nn - 1; i >= 0; i--) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_1(Wi);

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

    delete tmp_counts;
  }

  return counts;
}

labeled_vector *
beta_2(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nj, nn;
  int i;
  labeled_vector *counts, *tmp_counts;
  string key;

  labeled_vector_iter j;

  counts = new labeled_vector();

  if (W->children == NULL)
    return counts;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->name);
  key.append("_beta_2");

  (*counts)[key] = 0;

  for (i = nn - 1, nj = 0; i >= 0; i--, nj += ni) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_2(Wi);

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

    (*counts)[key] +=  (1 << nj);

    delete tmp_counts;
  }

  return counts;
}

labeled_vector *
beta_3(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nn;
  int i;
  labeled_vector *counts, *tmp_counts;
  string key;

  labeled_vector_iter j;

  counts = new labeled_vector();

  if (W->children == NULL)
    return counts;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->name);
  key.append("_beta_3");

  (*counts)[key] = 0;

  for (i = nn - 1; i >= 0; i--) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_3(Wi);

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

    (*counts)[key] +=  (1 << (n - ni));

    delete tmp_counts;
  }

  return counts;
}

labeled_vector *
default_basis(size_t max)
{
  labeled_vector *basis;
  basis = new labeled_vector();
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

labeled_vector *
count(Wht *W, size_t max)
{
  size_t k;
  labeled_vector *counts, *tmp_counts;

  counts = default_basis(max);

  tmp_counts = alpha_n(W);
  *counts += *tmp_counts;
  delete tmp_counts;

  tmp_counts = beta_1(W);
  *counts += *tmp_counts;
  delete tmp_counts;

  tmp_counts = beta_2(W);
  *counts += *tmp_counts;
  delete tmp_counts;

  tmp_counts = beta_3(W);
  *counts += *tmp_counts;
  delete tmp_counts;

  for (k = 1; k <= max; k++) {
    tmp_counts = alpha_k(W,k);
    *counts += *tmp_counts;
    delete tmp_counts;
  }

  return counts;
}

