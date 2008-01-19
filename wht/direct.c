/* Direct WHT
   ----------
   WHT is computed using ordinary matrix multiplication.
   The matrix, of course, is never built.
*/

/* wht_entry(k, row, col)
   ----------------------
     returns the entry of the WHT(2^k) at (row, col),
     1 <= row, col <= 2^k.
*/

#include "wht.h"

/**
 * \todo Investigate the stability of calculation method
 */
int 
wht_entry(int k, long row, long col)
{
  long h = (1 << (k-1));
  int  b = 0;

  /* case k = 1 */
  if (k == 1) {
    return (row == 2 && col == 2) ? -1 : 1;
  }

  /* recurse */
  if (row > h) {
    row = row - h;
    b   = 1;
  }
  if (col > h) {
    col = col - h;
    if (b)
      return -wht_entry(k-1, row, col);
  }
  return wht_entry(k-1, row, col);
}

static void 
apply_direct(Wht *W, long S, size_t D, wht_value *x) 
{
  long N = W->N;
  int n = W->n;
  wht_value *y = (wht_value *) i_malloc(N * sizeof(wht_value));
  long i, j;

  /* matrix multiplication, result is in y */
  for (i = 0; i < N; i++) {
    y[i] = 0;
    for (j = 0; j < N; j++)
      y[i] += wht_entry(n, i+1, j+1) * x[j*S];
  }

  /* copy y to x with stride S */
  for (i = 0; i < N; i++) 
    x[i*S] = y[i];

  i_free(y);
}

Wht *
wht_direct(size_t n)
{
  Wht *W;

  W        = node_init(n, "direct");
  W->apply = apply_direct;

  return W;
}

