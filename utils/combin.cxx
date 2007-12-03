#include <combin.h>

#include <stdlib.h>
#include <stdio.h>

#include <assert.h>

/**
 * \struct qn
 *
 * \brief Storage for rational integer
 */
struct qn {
  ulong numer;
  ulong denom;
};


/* From http://en.wikipedia.org/wiki/Binary_GCD_algorithm */
ulong 
gcd(ulong u, ulong v)
{
  int shift, diff;

  /* GCD(0,x) := x */
  if (u == 0 || v == 0)
    return u | v;

  /* Let shift := lg K, where K is the greatest power of 2
   * dividing both u and v. 
   */
  for (shift = 0; ((u | v) & 1) == 0; ++shift) {
    u >>= 1;
    v >>= 1;
  }

  while ((u & 1) == 0)
    u >>= 1;

  /* From here on, u is always odd. */
  do {
    while ((v & 1) == 0)  /* Loop X */
    v >>= 1;

    /* Now u and v are both odd, so diff(u, v) is even.
     * Let u = min(u, v), v = diff(u, v)/2. 
     */
    if (u <= v) {
      v -= u;
    } else {
      diff = u - v;
      u = v;
      v = diff;
    }

    v >>= 1;
  } while (v != 0);

  return u << shift;
}

struct qn 
qn_n_choose_k(ulong n, ulong k)
{
  struct qn qn_r, qn_p;

  ulong kp, nb, kb, ni, ki;
  ulong g1, g2, g3;

  /* Base case of recurence 
   * i.e ( n  n ) = 1
   */
  if (n == k) {
    qn_r.numer = 1;
    qn_r.denom = 1;
    return qn_r;
  }

  /* Base case of recurence 
   * i.e ( n  1 ) = n
   */
  if (k == 1) {
    qn_r.numer = n;
    qn_r.denom = 1;
    return qn_r;
  }

  /* Exploit the symetry of ( n  k ) == ( n  n - k ) */
  if (((n - k) > k) && (k > 1))
    kp = k;
  else
    kp = (n - k);

  g1 = gcd(n,kp);

  /* Normalize by GCD to save digits */
  nb = n  / g1;
  kb = kp / g1;

  qn_p = qn_n_choose_k(n - 1, kp - 1);

  ni = qn_p.numer;
  ki = qn_p.denom;

  g2 = gcd(nb, ki);
  g3 = gcd(ni, kb);

  /* Again, normalize by GCD to save digits 
   * i.e. (a / b) * (c / d) = 
   *  
   *  (a / gcd(a,d)) * (c / gcd(c,b)) 
   *  -------------------------------
   *  (b / gcd(c,b)) * (d / gcd(a,d)) 
   *
   */
  qn_r.numer = (nb / g2) * (ni / g3);
  qn_r.denom = (kb / g3) * (ki / g2);

  return qn_r;
}

/* NOTE: n = 31 and n = 38 are highest possible values that do not overflow
 * registers for 32 bit and 64 bit machines respectively 
 *
 * Could use Stirlings approximation to n! technique to empyrically dermine
 * this based on the register size.
 */
ulong
n_choose_k(ulong n, ulong k)
{
  assert(n >= k);
  struct qn qn_r;

  qn_r = qn_n_choose_k(n,k);

  /* Normalization by GCD should cause denominator to be 1 */
  assert (qn_r.denom == 1);

  /* A negative numerator implies overflow */
  assert (qn_r.numer >  0);

  return qn_r.numer;
}

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
  for (cmb_i = cmb_a->begin(), i = 1; cmb_i != cmb_a->end(); ++cmb_i, i++) 
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
  for (i = 0, cmb_i = cmb_a->begin(), cmb_j = cmb_b->begin(); i < k; i++, ++cmb_i, ++cmb_j) 
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
  for (i = 0, cmb_i = cmb_a->begin(); i < m; i++, ++cmb_i);

  return cmb_i;
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
  for (cmb_i = cmb->begin(), cmp_i = cmp->begin(); cmb_i != cmb->end(); ++cmb_i, ++cmp_i) {
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
  size_t min_fp;

  cpn = new compos_node();
  cpn->value = n;
  cpn->children = new compos_nodes();

  if (n >= min_n && n <= max_n)
    return cpn;

  /* If n is greater than the largest element, we need to generate a
   * composition of at least 2 elements */
  if (n >= max_n) 
    min_fp = 2;
  else
    min_fp = min_f;

  cmp = compos_rand(n, elem_min(n,min_fp), elem_min(n,max_f));

  if (cmp->size() == 1) /* One element in composition => no children */
    return cpn;

  cpn->children->resize(cmp->size());

  for (cmp_i = cmp->begin(), cpn_i = cpn->children->begin(); cmp_i != cmp->end(); ++cmp_i, ++cpn_i) 
    *cpn_i = compos_tree_rand(*cmp_i, min_f, max_f, min_n, max_n);

  delete cmp;

  return cpn;
}

compos_node *
compos_tree_rand_right(uint n, uint min_f, uint max_f, uint min_n, uint max_n)
{
  compos *cmp;
  compos::iterator cmp_i;
  compos::iterator cmp_j;

  compos_node *cpn;
  compos_nodes::iterator cpn_i;
  size_t min_fp;
  bool rcond;

  cpn = new compos_node();
  cpn->value = n;
  cpn->children = new compos_nodes();

  if (n >= min_n && n <= max_n)
    return cpn;

  /* If n is greater than the largest element, we need to generate a
   * composition of at least 2 elements */
  if (n >= max_n) 
    min_fp = 2;
  else
    min_fp = min_f;

  cmp = NULL;

  /* NOTE: Possible to get rid of this non deterministic loop mechanism,
   * if compos_rand can restrict size of elements to between min_n and max_n
   */
  do {
    if (cmp != NULL) delete cmp;

    cmp = compos_rand(n, elem_min(n,min_fp), elem_min(n,max_f));

    rcond = true;

    /* Check to make sure that each non right children will be a leaf node, i.e
     * the only node with children is the rightmost.  
     */
    cmp_j = --(cmp->end());
    for (cmp_i = cmp->begin(); cmp_i != cmp_j; ++cmp_i) 
      rcond = rcond && (*cmp_i >= min_n) && (*cmp_i <= max_n);

  } while (! rcond);

  if (cmp->size() == 1) /* One element in composition => no children */
    return cpn;

  cpn->children->resize(cmp->size());

  for (cmp_i = cmp->begin(), cpn_i = cpn->children->begin(); cmp_i != cmp->end(); ++cmp_i, ++cpn_i) 
    *cpn_i = compos_tree_rand_right(*cmp_i, min_f, max_f, min_n, max_n);

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

  for (cpns_i = cpns->begin(); cpns_i != cpns->end(); ++cpns_i)
    compos_tree_free(*cpns_i);

  delete cpns;
  delete cpn;
}

Wht *
compos_tree_to_wht(compos_node *cpn)
{
  size_t i;
  compos_nodes *cpns;
  compos_nodes::reverse_iterator cpns_i;

  Wht *Ws[MAX_SPLIT_NODES];
  cpns = cpn->children;

  if (cpns->empty()) {
    return small_init(cpn->value);
  } else {
    /* NOTE: Antilexigraphic ordering, i.e. WHT nodes are stored right to left */
    for (cpns_i = cpns->rbegin(), i = 0; cpns_i != cpns->rend(); ++cpns_i, i++)
      Ws[i] = compos_tree_to_wht(*cpns_i);

    return split_init(Ws, cpns->size());
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
