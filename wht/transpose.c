#include "wht.h"

void
transpose(wht_value *x, size_t m, size_t n)
{
  size_t i, j;
  wht_value *y;

  y = i_malloc(m*n * sizeof(*x));

  for (i = 0; i < m*n; i++)
    y[i] = x[i]; 

  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      x[j*m + i] = y[i*n + j];
    }
  }

  free(y);
}

bool
transpose_accept(Wht *W)
{
  return (W->children->nn == 2) && split_accept(W);
}

void 
transpose_apply(Wht *W, long S, size_t D, wht_value *x)
{
  size_t N, N1, N2, j;
  wht_value *xs;

  N  = W->N;
  N1 = W->children->ns[0];
  N2 = W->children->ns[1];

  /* Compute right transform first */
  for (j = 0, xs = x; j < N1; j++, xs += (N2 * S))
    (W->children->Ws[1]->apply)(W->children->Ws[1], S, 0, xs);

  /* L_N^N2 */
  transpose(x, N1, N2);

  /* Compute transposed left transform */
  for (j = 0, xs = x; j < N2; j++, xs += (N1 * S))
    (W->children->Ws[0]->apply)(W->children->Ws[0], S, 0, xs);

  /* L_N^N1 */
  transpose(x, N2, N1);
}

Wht *
transpose_init(char *name, Wht *Ws[], size_t nn, int params[], size_t np) 
{
  Wht *W;

  W            = split_init(name, Ws, nn, params, np);
  W->apply     = transpose_apply;
  W->accept    = transpose_accept;

  return W;  
}

