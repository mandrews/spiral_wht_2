extern "C" {
#include "wht.h"
#include "measure.h"
#include "linalg.h"
}

#include <stdlib.h>

#include <string>
#include <map>
#include <vector>

using std::string;
using std::vector;
using std::map;

typedef map<const string, size_t> count_set;
typedef map<const string, size_t>::iterator count_set_iter;

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

void
matrix_add_row_as_count_set(struct matrix *a, size_t k, count_set *r)
{
  size_t n, j;
  count_set_iter i;

  n = a->n;

  assert(k <= n);

  for (j = 0, i = r->begin(); j < n && i != r->end(); j++, i++) 
    matrix_elem(a,k,j) = i->second;
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

    count_set_add(counts, tmp_counts);

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
  size_t n, ni, nn;
  int i;
  count_set *counts, *tmp_counts;
  string key;

  count_set_iter j;

  counts = new count_set();

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
  size_t n, ni, nn;
  int i;
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

  for (i = nn - 1; i >= 0; i--) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_1(Wi);

    (*counts)[key] += ((1 << (n - ni)) * (*tmp_counts)[key]);
    tmp_counts->erase(key);

    count_set_add(counts, tmp_counts);

    delete tmp_counts;
  }

  return counts;
}

count_set *
beta_2(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nj, nn;
  int i;
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

  for (i = nn - 1, nj = 0; i >= 0; i--, nj += ni) {
    Wi = W->children->Ws[i];
    ni = Wi->n;

    tmp_counts = beta_2(Wi);

    (*counts)[key] += ((1 << (n - ni)) * (*tmp_counts)[key]);
    (*counts)[key] +=  (1 << nj);

    tmp_counts->erase(key);

    count_set_add(counts, tmp_counts);

    delete tmp_counts;
  }

  return counts;
}

count_set *
beta_3(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nn;
  int i;
  count_set *counts, *tmp_counts;
  string key;

  counts = new count_set();

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

    (*counts)[key] += ((1 << (n - ni)) * (*tmp_counts)[key]);
    (*counts)[key] +=  (1 << (n - ni));

    tmp_counts->erase(key);

    count_set_add(counts, tmp_counts);

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
  (*counts)["small[1]"] = 0;
  (*counts)["small[2]"] = 0;
  (*counts)["small[3]"] = 0;
  (*counts)["small[4]"] = 0;
  (*counts)["split_alpha"] = 0;
  (*counts)["split_beta_1"] = 0;
  (*counts)["split_beta_2"] = 0;
  (*counts)["split_beta_3"] = 0;

  tmp_counts = alpha_n(W);
  count_set_add(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_1(W);
  count_set_add(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_2(W);
  count_set_add(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_3(W);
  count_set_add(counts, tmp_counts);
  delete tmp_counts;

  for (k = 1; k <= max; k++) {
    tmp_counts = alpha_k(W,k);
    count_set_add(counts, tmp_counts);
    delete tmp_counts;
  }

  return counts;
}

vector<double> *
calc_coeffs()
{
  char *basis[] = {
    "small[1]",
    "small[2]",
    "small[3]",
    "small[4]",
    "split[small[1],small[1],small[1]]",
    "split[split[small[1]],small[1],split[small[1]]]",
    "split[small[1],split[small[1],small[1]]]",
    "split[split[small[1],small[1]],small[1]]",
    NULL
  };
  const size_t m = 8;

  char **elem;
  size_t max, k;
  vector<double> *coeffs;
  struct stat *stat;
  count_set *counts;
  struct matrix *a, *b, *c;
  Wht *W;

  max = 4;

  coeffs = new vector<double>();

  a = matrix_init(m,m);
  b = matrix_init(m,1);

  for (elem = basis, k = 0; *elem != NULL; elem++, k++) {
    W = wht_parse(*elem);
    //printf("elem: %s\n", W->to_string(W));
    counts = ic_counts(W, max);
    stat = measure(W, "TOT_INS");

    matrix_add_row_as_count_set(a, k, counts);
    matrix_elem(b,k,0) = stat->mean - 238; /** \todo allow value */

    wht_free(W);
    delete counts;
    free(stat);
  }

  matrix_print(a);
  matrix_print(b);

  c = matrix_least_squares_error(a,b);

  printf("\n");

  matrix_print(c);

  matrix_free(a);
  matrix_free(b);
  matrix_free(c);

  return coeffs;
}

int 
main()
{
  Wht *W;
  char *plan;
  count_set *counts;
  count_set_iter i;
  vector<double> *coeffs;

  coeffs = calc_coeffs();
  delete coeffs;

#if 0
  plan = (char *) malloc(sizeof(char) * 255);

  scanf("%s\n", plan);

  // printf("%s\n", plan);
  W = wht_parse(plan);

  if (wht_accept(W) == false) 
    wht_error(wht_error_msg);

  counts = ic_counts(W, 4);

  for (i = counts->begin(); i != counts->end(); i++) {
    printf("%s : %zd\n", i->first.c_str(), i->second);
  }
  delete counts;

  free(plan);
  wht_free(W);
#endif

  return 0;
}
