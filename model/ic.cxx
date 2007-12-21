extern "C" {
#include "wht.h"
#include "measure.h"
#include "linalg.h"
}

#include "labeled_vector.h"

double
ic_predict(labeled_vector *counts, labeled_vector *coeffs);

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
labeled_vector_init()
{
  labeled_vector *counts;
  counts = new labeled_vector();

  (*counts)["small[1]"] = 0.0;
  (*counts)["small[2]"] = 0.0;
  (*counts)["small[3]"] = 0.0;
  (*counts)["small[4]"] = 0.0;
  (*counts)["smallil(2)[1]"] = 0.0;
  (*counts)["smallil(2)[2]"] = 0.0;
  (*counts)["smallil(2)[3]"] = 0.0;
  (*counts)["smallil(2)[4]"] = 0.0;
  (*counts)["smallil(4)[1]"] = 0.0;
  (*counts)["smallil(4)[2]"] = 0.0;
  (*counts)["smallil(4)[3]"] = 0.0;
  (*counts)["smallil(4)[4]"] = 0.0;
  (*counts)["smallil(8)[1]"] = 0.0;
  (*counts)["smallil(8)[2]"] = 0.0;
  (*counts)["smallil(8)[3]"] = 0.0;
  (*counts)["smallil(8)[4]"] = 0.0;
#if 0
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
#endif
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

labeled_vector *
ic_counts(Wht *W, size_t max)
{
  size_t k;
  labeled_vector *counts, *tmp_counts;

  counts = labeled_vector_init();

  tmp_counts = alpha_n(W);
  labeled_vector_add(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_1(W);
  labeled_vector_add(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_2(W);
  labeled_vector_add(counts, tmp_counts);
  delete tmp_counts;

  tmp_counts = beta_3(W);
  labeled_vector_add(counts, tmp_counts);
  delete tmp_counts;

  for (k = 1; k <= max; k++) {
    tmp_counts = alpha_k(W,k);
    labeled_vector_add(counts, tmp_counts);
    delete tmp_counts;
  }

  return counts;
}

labeled_vector *
calc_coeffs()
{
  char *basis[] = {
    "small[1]",
    "small[2]",
    "small[3]",
    "small[4]",
    "splitil[smallil(2)[1],small[1]]",
    "splitil[smallil(2)[2],small[1]]",
    "splitil[smallil(2)[3],small[1]]",
    "splitil[smallil(2)[4],small[1]]",
    "splitil[smallil(4)[1],small[1]]",
    "splitil[smallil(4)[2],small[1]]",
    "splitil[smallil(4)[3],small[1]]",
    "splitil[smallil(4)[4],small[1]]",
    "splitil[smallil(8)[1],small[1]]",
    "splitil[smallil(8)[2],small[1]]",
    "splitil[smallil(8)[3],small[1]]",
    "splitil[smallil(8)[4],small[1]]",
#if 0
    "smallv(2)[2]",
    "smallv(2)[3]",
    "smallv(2)[4]",
    "splitil[smallv(2,2,1)[2],small[1]]",
    "splitil[smallv(2,2,1)[3],small[1]]",
    "splitil[smallv(2,2,1)[4],small[1]]",
    "splitil[smallv(2,4,1)[2],small[1]]",
    "splitil[smallv(2,4,1)[3],small[1]]",
    "splitil[smallv(2,4,1)[4],small[1]]",
    "splitil[smallv(2,8,1)[2],small[1]]",
    "splitil[smallv(2,8,1)[3],small[1]]",
    "splitil[smallv(2,8,1)[4],small[1]]",
#endif
    "split[small[1],small[1],small[1]]",
    "split[split[small[1]],small[1],split[small[1]]]",
    "split[small[1],split[small[1],small[1]]]",
    "split[split[small[1],small[1]],small[1]]",
    "splitil[small[1],small[1],small[1]]",
    "splitil[splitil[small[1]],small[1],splitil[small[1]]]",
    "splitil[small[1],splitil[small[1],small[1]]]",
    "splitil[splitil[small[1],small[1]],small[1]]",
    NULL
  };

  char **elem;
  size_t max, k, m;
  struct stat *stat;
  labeled_vector *counts, *coeffs;
  struct matrix *a, *b, *c;
  double y;
  Wht *W;

  max = 4;

  coeffs = labeled_vector_init();

  m = coeffs->size();

  a = matrix_init(m,m);
  b = matrix_init(m,1);

  for (elem = basis, k = 0; *elem != NULL; elem++, k++) {
    W = wht_parse(*elem);
    
    counts = ic_counts(W, max);

    stat = measure(W, "PAPI", "TOT_INS", true, 1, 1);

    y = stat->mean;

		labeled_vector_to_matrix_row(counts, a, k);
    matrix_elem(b,k,0) = y;

    wht_free(W);
    delete counts;
    free(stat);
  }

  c = matrix_least_squares_error(a,b);

	matrix_col_to_labeled_vector(c,0,coeffs);

  matrix_free(a);
  matrix_free(b);
  matrix_free(c);

  return coeffs;
}

double
ic_predict(labeled_vector *counts, labeled_vector *coeffs)
{
  labeled_vector_iter i, j;
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
  labeled_vector *counts;
  labeled_vector_iter i;
  labeled_vector *coeffs;

  coeffs = calc_coeffs();

#if 1
  printf("coeffs:\n");
  for (i = coeffs->begin(); i != coeffs->end(); i++) {
    printf("%s : %g\n", i->first.c_str(), i->second);
  }
#endif

  plan = (char *) malloc(sizeof(char) * 255);

  while (fgets(plan, 255, stdin) != NULL) {

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
    fflush(stdout);

    wht_free(W);
  }

  free(plan);
  delete coeffs;

  return 0;
}
