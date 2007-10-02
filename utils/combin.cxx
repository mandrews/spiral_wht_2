#include <combin.h>

#include <stdlib.h>
#include <stdio.h>

static memo_fact_table memo_fact; /**< Factorial memoization table */

combin *
combin_k_rand(uint n, uint k)
{
  assert(n >= k);

  combin *cmb_a, *cmb_b;
  combin::iterator cmb_i, cmb_j;
  uint tmp, i;

  cmb_b = new combin(k);

  if (k == 0) /* Return the empty combination */
    return cmb_b;

  cmb_a = new combin(n);

  /* Initialize a to 1 .. n-1 */
  for (cmb_i = cmb_a->begin(), i = 1; cmb_i != cmb_a->end(); cmb_i++, i++) 
    *cmb_i = i;

  /* Perform a random swap between 1 .. n-1, 
   * then between 1 .. n-2, ...,  
   * and finally between 1 .. k-1 
   */
  for (i = n-1; i > k; i--) {
    cmb_i = combin_elem_rand(cmb_a, i);
    cmb_j = combin_elem_rand(cmb_a, i);

    /* Swap values at iterator positions */
    tmp = *cmb_i;
    *cmb_i = *cmb_j;
    *cmb_j = tmp;
  }

  /* Place the first k elements of a into b*/
  for (i = 0, cmb_i = cmb_a->begin(), cmb_j = cmb_b->begin(); i < k; i++, cmb_i++, cmb_j++) 
    *cmb_j = *cmb_i;

  delete cmb_a;

  cmb_b->sort();

  return cmb_b;
}

combin::iterator
combin_elem_rand(combin * cmb_a, uint n)
{
  size_t m, i;
  combin::iterator cmb_i;

  m = random() % n;
  for (i = 0, cmb_i = cmb_a->begin(); i < m; i++, cmb_i++);

  return cmb_i;
}

uint
fact(uint x)
{
  uint fx;

  if (x < 2)
    return 1;

  memo_fact_table::iterator v;
  v = NULL;
  v = memo_fact.find(x);

  if (v != memo_fact.end())
    return v->second;

  fx = x * fact(x - 1);

  memo_fact[x] = fx;

  return fx;
}

uint
n_choose_k(uint n, uint k)
{
  assert(n >= k);
  return fact(n) / (fact(k) * fact(n-k));
}

combin *
combin_rand(uint n, uint a, uint b)
{
  combin_pdf pdf, cdf;
  uint k;
  double accum, sum, r;

  sum   = 0;
  accum = 0.0;

  for (k = a; k <= b; k++) {
    pdf[k] = n_choose_k(n,k);
    sum += pdf[k];
  }

  for (k = a; k <= b; k++) {
    cdf[k]  = pdf[k] / sum;
    cdf[k] += accum;
    accum  = cdf[k];
  }

  r = ((double) random() / (double) RAND_MAX);

  for (k = a; k <= b; k++) 
    if (r < cdf[k]) 
      break;

  return combin_k_rand(n,k);
}

compos *
combin_to_compos(uint n, combin *cmb)
{
  compos *cmp;
  combin::iterator cmb_i;
  compos::iterator cmp_i;
  uint accum;

  cmp = new compos(cmb->size() + 1);

  if (cmb->empty()) {
    cmp_i = cmp->begin();
    *cmp_i = n;
    return cmp;
  }

  accum = 0;
  for (cmb_i = cmb->begin(), cmp_i = cmp->begin(); cmb_i != cmb->end(); cmb_i++, cmp_i++) {
    *cmp_i = *cmb_i - accum;
    accum = *cmb_i;
  }

  *cmp_i = n - accum;

  return cmp;
}

compos *
compos_rand(uint n, uint a, uint b)
{
  combin *cmb_a;
  compos *cmp_a;

  cmb_a = combin_rand(n-1, a-1, b-1);
  cmp_a = combin_to_compos(n, cmb_a);

  delete cmb_a;
  return cmp_a;
}

#define elem_min(a,b) ((a < b) ? a : b)

compos_node *
compos_tree_rand(uint n, uint min_f, uint max_f, uint min_n, uint max_n)
{
  compos *cmp;
  compos::iterator cmp_i;

  compos_node *cpn;
  compos_nodes::iterator cpn_i;

  cpn = new compos_node();
  cpn->value = n;
  cpn->children = new compos_nodes();

  if (n <= min_n)
    return cpn;

  cmp = compos_rand(n, elem_min(n,min_f), elem_min(n,max_f));

  if (cmp->size() == 1) /* One element in composition => no children */
    return cpn;

  cpn->children->resize(cmp->size());

  for (cmp_i = cmp->begin(), cpn_i = cpn->children->begin(); cmp_i != cmp->end(); cmp_i++, cpn_i++) 
    *cpn_i = compos_tree_rand(*cmp_i, min_f, max_f, min_n, max_n);

  delete cmp;

  return cpn;
}

#undef elem_min

void
compos_tree_free(compos_node *cpn)
{
  compos_nodes *cpns;
  compos_nodes::iterator cpns_i;

  cpns = cpn->children;

  for (cpns_i = cpns->begin(); cpns_i != cpns->end(); cpns_i++)
    compos_tree_free(*cpns_i);

  delete cpns;
  delete cpn;
}

Wht *
compos_tree_to_wht(compos_node *cpn)
{
  size_t i;
  compos_nodes *cpns;
  compos_nodes::iterator cpns_i;

  small_init_fp f;
  split_init_fp g;

  Wht *Ws[MAX_SPLIT_NODES];
  cpns = cpn->children;

  if (cpns->empty()) {
    f = small_lookup("small", 0);
    return (f)("small", cpn->value, NULL, 0);
  } else {
    for (cpns_i = cpns->begin(), i = 0; cpns_i != cpns->end(); cpns_i++, i++)
      Ws[i] = compos_tree_to_wht(*cpns_i);

    g = split_lookup("split", 0);
    return (g)("split", Ws, cpns->size(), NULL, 0);
  }
}

char* 
compos_tree_to_string(compos_node *cpn)
{
  Wht *W;
  char *buf;

  W = compos_tree_to_wht(cpn);

  buf = W->to_string(W);
  W->free(W);

  return buf;
}
