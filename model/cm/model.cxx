#include "model.h"

long
cm_count(Wht *W, long R, long C, long b, long a)
{
  int i, nn;
  long cm, N, Ni, Nt;
  Wht *Wi;

  long bp = (long) ceil(((double) b) / ((double) R));
  long ap = (long) ceil(((double) C) / ( ((double) a) / ((double) R)));

  N = W->N;

  if (N*R <= ap*a)
    return N/bp;

  if (W->children == NULL) {
    if ((R >= C) && (a == 1))
      return 3*N;
    else if ((R < C) && (a == 1))
      return 2*N/bp;
    else /* A > 1 */
      return 2*N/bp;
  } else {
    nn = W->children->nn;
    cm = 0;
    Nt = 1;

    for (i = 0; i < nn; i++) {
      Wi = W->children->Ws[i];
      Ni = Wi->N;
      cm += N/Ni * cm_count(Wi, Nt*R, C, b, a); 
      Nt *= Ni;
    }

    return cm;
  }
}
