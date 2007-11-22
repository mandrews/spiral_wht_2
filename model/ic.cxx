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

/* Annotated matrix */

typedef map<const string, double> count_set;
typedef map<const string, double>::iterator count_set_iter;

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

void
col_to_count_set(struct matrix *a, size_t k, count_set *r)
{
  size_t m, j;
  count_set_iter i;

  m = a->m;

  assert(k <= m);

  for (j = 0, i = r->begin(); j < m && i != r->end(); j++, i++) 
    i->second = matrix_elem(a,j,k);
}

count_set *
alpha_n(Wht *W)
{
  Wht *Wi;
  size_t n, ni, nn, i;
  count_set *counts, *tmp_counts;
  string key;

  count_set_iter j;

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

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

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

    int il = 1;
    if (Wi->attr[interleave_by] != UNSET_PARAMETER)
      il = Wi->attr[interleave_by];

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += (((1 << (n - ni)) * j->second) / il);

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

  count_set_iter j;

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

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

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

  count_set_iter j;

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

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

    (*counts)[key] +=  (1 << nj);

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

  count_set_iter j;

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

    for (j = tmp_counts->begin(); j != tmp_counts->end(); j++) 
      (*counts)[j->first] += ((1 << (n - ni)) * (*tmp_counts)[j->first]);

    (*counts)[key] +=  (1 << (n - ni));

    delete tmp_counts;
  }

  return counts;
}

count_set *
count_set_init()
{
  count_set *counts;
  counts = new count_set();

  (*counts)["small[1]"] = 0.0;
  (*counts)["small[2]"] = 0.0;
  (*counts)["small[3]"] = 0.0;
  (*counts)["small[4]"] = 0.0;
  (*counts)["smallil(2)[2]"] = 0.0;
  (*counts)["smallil(2)[3]"] = 0.0;
  (*counts)["smallil(2)[4]"] = 0.0;
  (*counts)["smallil(4)[2]"] = 0.0;
  (*counts)["smallil(4)[3]"] = 0.0;
  (*counts)["smallil(4)[4]"] = 0.0;
  (*counts)["smallil(8)[2]"] = 0.0;
  (*counts)["smallil(8)[3]"] = 0.0;
  (*counts)["smallil(8)[4]"] = 0.0;
  (*counts)["smallv(2)[2]"] = 0.0;
  (*counts)["smallv(2)[3]"] = 0.0;
  (*counts)["smallv(2)[4]"] = 0.0;
  (*counts)["smallv(2,2,1)[2]"] = 0.0;
  (*counts)["smallv(2,2,1)[3]"] = 0.0;
  (*counts)["smallv(2,2,1)[4]"] = 0.0;
  (*counts)["smallv(2,4,1)[2]"] = 0.0;
  (*counts)["smallv(2,4,1)[3]"] = 0.0;
  (*counts)["smallv(2,4,1)[4]"] = 0.0;
  (*counts)["smallv(2,8,1)[2]"] = 0.0;
  (*counts)["smallv(2,8,1)[3]"] = 0.0;
  (*counts)["smallv(2,8,1)[4]"] = 0.0;
  (*counts)["split_alpha"] = 0.0;
  (*counts)["split_beta_1"] = 0.0;
  (*counts)["split_beta_2"] = 0.0;
  (*counts)["split_beta_3"] = 0.0;
  (*counts)["splitil_alpha"] = 0.0;
  (*counts)["splitil_beta_1"] = 0.0;
  (*counts)["splitil_beta_2"] = 0.0;
  (*counts)["splitil_beta_3"] = 0.0;

  return counts;
}

count_set *
ic_counts(Wht *W, size_t max)
{
  size_t k;
  count_set *counts, *tmp_counts;

  counts = count_set_init();

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

count_set *
calc_coeffs()
{
  char *basis[] = {
    "small[1]",
    "small[2]",
    "small[3]",
    "small[4]",
    "split[small[2],small[2],small[2]]",
    "split[split[small[2]],small[2],split[small[2]]]",
    "split[small[2],split[small[2],small[2]]]",
    "split[split[small[2],small[2]],small[2]]",
    "splitil[small[2],small[2],small[2]]",
    "splitil[splitil[small[2]],small[2],splitil[small[2]]]",
    "splitil[small[2],splitil[small[2],small[2]]]",
    "splitil[splitil[small[2],small[2]],small[2]]",
    "splitil[smallil(2)[2],smallil(2)[2],small[1]]",
    "splitil[smallil(2)[3],smallil(2)[3],small[1]]",
    "splitil[smallil(2)[4],smallil(2)[4],small[1]]",
    "splitil[smallil(4)[2],smallil(4)[2],small[1]]",
    "splitil[smallil(4)[3],smallil(4)[3],small[1]]",
    "splitil[smallil(4)[4],smallil(4)[4],small[1]]",
    "splitil[smallil(8)[2],smallil(8)[2],small[1]]",
    "splitil[smallil(8)[3],smallil(8)[3],small[1]]",
    "splitil[smallil(8)[4],smallil(8)[4],small[1]]",
    "smallv(2)[2]",
    "smallv(2)[3]",
    "smallv(2)[4]",
    "splitil[smallv(2,2,1)[2],smallv(2,2,1)[2],small[1]]",
    "splitil[smallv(2,2,1)[3],smallv(2,2,1)[3],small[1]]",
    "splitil[smallv(2,2,1)[4],smallv(2,2,1)[4],small[1]]",
    "splitil[smallv(2,4,1)[2],smallv(2,4,1)[2],small[2]]",
    "splitil[smallv(2,4,1)[3],smallv(2,4,1)[3],small[2]]",
    "splitil[smallv(2,4,1)[4],smallv(2,4,1)[4],small[2]]",
    "splitil[smallv(2,8,1)[2],smallv(2,8,1)[2],small[3]]",
    "splitil[smallv(2,8,1)[3],smallv(2,8,1)[3],small[3]]",
    "splitil[smallv(2,8,1)[4],smallv(2,8,1)[4],small[3]]",
    NULL
  };

  char **elem;
  size_t max, k, m;
  struct stat *stat;
  count_set *counts, *coeffs;
  struct matrix *a, *b, *c;
  Wht *W;

  max = 4;

  coeffs = count_set_init();
  m = coeffs->size();

  a = matrix_init(m,m);
  b = matrix_init(m,1);

  for (elem = basis, k = 0; *elem != NULL; elem++, k++) {
    W = wht_parse(*elem);
    //printf("elem: %s\n", W->to_string(W));
    counts = ic_counts(W, max);
    stat = measure(W, "PAPI", "TOT_INS", 1);

    matrix_add_row_as_count_set(a, k, counts);
    matrix_elem(b,k,0) = stat->mean - 236; /** \todo allow value */

    // if (k == 0) print_labels(counts);

    wht_free(W);
    delete counts;
    free(stat);
  }

#if 0
  matrix_print(a);
  matrix_print(b);
  printf("\n");
  matrix_print(c);
#endif

  c = matrix_least_squares_error(a,b);

  col_to_count_set(c,0,coeffs);

  matrix_free(a);
  matrix_free(b);
  matrix_free(c);

  return coeffs;
}

double
ic_predict(count_set *counts, count_set *coeffs)
{
  count_set_iter i, j;
  double sum;

  i = counts->begin();
  j = coeffs->begin();
  sum = 0.0;
  for (; i != counts->end() && j != counts->end(); i++, j++) {
    sum += i->second * j->second;
  }

  return sum;
}


int 
main()
{
  Wht *W;
  char *plan;
  count_set *counts;
  count_set_iter i;
  count_set *coeffs;

  coeffs = calc_coeffs();

#if 0
  printf("coeffs:\n");
  for (i = coeffs->begin(); i != coeffs->end(); i++) {
    printf("%s : %g\n", i->first.c_str(), i->second);
  }
#endif

  plan = (char *) malloc(sizeof(char) * 255);

  while (gets(plan) != NULL) {

    W = wht_parse(plan);

    if (wht_error_msg(W) != NULL) {
      printf("rejected, %s\n", wht_error_msg(W));
      wht_free(W);
      exit(1);
    }

    counts = ic_counts(W, 4);

#if 0
    printf("counts:\n");
    for (i = counts->begin(); i != counts->end(); i++) {
      printf("%s : %g\n", i->first.c_str(), i->second);
    }
    printf("instructions:\n");
#endif

    printf("%g\n", ic_predict(counts, coeffs));
    wht_free(W);
  }

  free(plan);
  delete coeffs;

  return 0;
}
