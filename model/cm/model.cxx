#include "model.h"

long
cm_count(Wht *W, long R, long C)
{
  int i, nn;
  long cm, N, Ni, Nt;
  Wht *Wi;

  N = W->N;

  if (W->children == NULL) {
    if (N <= floor(C / R))
      return N;
    else if (R >= C)
      return 3*N;
    else /* R < C */
      return 2*N;
  } else {
    nn = W->children->nn;
    cm = 0;
    Nt = 1;

    for (i = 0; i < nn; i++) {
      Wi = W->children->Ws[i];
      Ni = Wi->N;
      cm += N/Ni * cm_count(Wi, Nt * R, C);
      Nt *= Ni;
    }

    return cm;
  }
}
