extern "C" {
#include "wht.h"
}

#include <stdlib.h>

#include <string>
#include <map>

typedef std::map<const std::string, size_t> count_set;
typedef std::map<const std::string, size_t>::iterator count_set_iter;

using std::string;

void
count_set_merge(count_set *into, count_set *from)
{
  count_set_iter i;

  for (i = from->begin(); i != from->end(); i++)
    (*into)[i->first] = i->second;

}

void
count_set_add(count_set *into, count_set *from)
{
  count_set_iter i;

  for (i = from->begin(); i != from->end(); i++)
    (*into)[i->first] += i->second;

}

count_set *
alpha_n(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nn, i;
  count_set *counts, *tmp_counts;
  string key;

  counts = new count_set();

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

    (*counts)[key] += ((1 << (n - ni)) * (*tmp_counts)[key]);
    tmp_counts->erase(key);

    count_set_merge(counts, tmp_counts);

    delete tmp_counts;
  }

  return counts;
}

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

count_set *
alpha_k(Wht *W, size_t k)
{
  Wht *Wi;
  size_t n, ni, nn, i;
  count_set *counts, *tmp_counts;
  string key;

  count_set_iter j;

  counts = new count_set();

  if (W->children == NULL) {
    key = small_to_key(W);

    if (W->n == k)
      (*counts)[key] = 1;
    else
      (*counts)[key] = 0;

    return counts;
  }

  n  = W->n;
  nn = W->children->nn;

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = alpha_k(Wi,k);

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * j->second);

    delete tmp_counts;
  }

  return counts;
}


count_set *
beta_1(Wht *W)
{

  Wht *Wi;
  size_t n, ni, nn, i;
  count_set *counts, *tmp_counts;
  string key;

  counts = new count_set();

  if (W->children == NULL)
    return counts;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->name);
  key.append("_beta_1");

  (*counts)[key] = nn;

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_1(Wi);

    (*counts)[key] += ((1 << (n - ni)) * (*tmp_counts)[key]);
    tmp_counts->erase(key);

    count_set_merge(counts, tmp_counts);

    delete tmp_counts;
  }

  return counts;
}

count_set *
beta_2(Wht *W)
{

  Wht *Wi;
  size_t n, ni, nj, nn, i;
  count_set *counts, *tmp_counts;
  string key;

  counts = new count_set();

  if (W->children == NULL)
    return counts;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->name);
  key.append("_beta_2");

  (*counts)[key] = 0;

  for (i = 0, ni = 0, nj = 0; i < nn; i++, nj += ni) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_2(Wi);

    (*counts)[key] += ((1 << (n - ni)) * (*tmp_counts)[key]);
    (*counts)[key] += (1 << nj);

    tmp_counts->erase(key);

    count_set_merge(counts, tmp_counts);

    delete tmp_counts;
  }

  return counts;
}

count_set *
beta_3(Wht *W)
{

  Wht *Wi;
  size_t n, ni, nn, i;
  count_set *counts, *tmp_counts;
  string key;

  counts = new count_set();

  if (W->children == NULL)
    return counts;

  n  = W->n;
  nn = W->children->nn;

  key.append(W->name);
  key.append("_beta_3");

  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_3(Wi);

    (*counts)[key] += ((1 << (n - ni)) * (*tmp_counts)[key]);
    (*counts)[key] +=  (1 << (n - ni));

    tmp_counts->erase(key);

    count_set_merge(counts, tmp_counts);

    delete tmp_counts;
  }

  return counts;
}

count_set *
ic_counts(Wht *W, size_t max)
{
  size_t k;
  count_set *counts, *tmp_counts;

  counts = new count_set();

  tmp_counts = alpha_n(W);
  count_set_merge(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_1(W);
  count_set_merge(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_2(W);
  count_set_merge(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_3(W);
  count_set_merge(counts, tmp_counts);
  delete tmp_counts;

  for (k = 0; k <= max; k++) {
    tmp_counts = alpha_k(W,k);
    count_set_add(counts, tmp_counts);
    delete tmp_counts;
  }

  return counts;
}

int 
main()
{
  Wht *W;
  char *plan;
  count_set *counts;
  count_set_iter i;
  int exit = 0;

  plan = (char *) malloc(sizeof(*plan) * 255);

  scanf("%s\n", plan);

  // printf("%s\n", plan);

  W = wht_parse(plan);

  if (wht_accept(W) == false) {
    printf("%s\n", wht_error_msg);
    exit = 1;
    goto DONE;
  }

  counts = ic_counts(W, 4);

  for (i = counts->begin(); i != counts->end(); i++) {
    printf("%s : %zd\n", i->first.c_str(), i->second);
  }

  delete counts;

DONE:
  free(plan);
  wht_free(W);

  return exit;
}
