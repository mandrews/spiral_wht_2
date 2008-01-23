#include "wht.h"

#ifdef WHT_HAVE_OMP
#include <omp.h>
#endif/*WHT_HAVE_OMP*/

#define OMP_CHUNK   (256)

void split_concurrent_apply(Wht *W, long S, size_t U, wht_value *x);

void
split_concurrent_rule(Wht *W)
{
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be interleaved");

  W->apply = split_concurrent_apply;
}

void 
split_concurrent_apply(Wht *W, long S, size_t U, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;
  Wht *Wi;

  nn = W->children->nn;

  N  = W->N;
  R  = N;
  S1 = 1;

  /* Iterate over children WHTs, stored anti lexigraphically  */
  for (i = 0; i < nn; i++) {
    Ni = W->children->ns[i];
    Wi = W->children->Ws[i];
    R /= Ni;

    #pragma omp parallel for private(k) schedule(guided, OMP_CHUNK)
    for (j = 0; j < R; j++) 
      for (k = 0; k < S1; k++) 
        (Wi->apply)(Wi, S1*S, S, x+k*S+j*Ni*S1*S);

    S1 *= Ni;
  }
}
