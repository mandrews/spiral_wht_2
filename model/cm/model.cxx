#include "model.h"
#if 0
long 
sb_1(long cost, long R, long C, long b)
{
  return cost;
}

long 
sb_2(long cost, long R, long C, long b)
{
  return cost;
}
#endif

#if 1
long 
sb_2(long cost, long R, long C, long b)
{
  long d;
  
  d = (long) ceil((R*1.0)/b);

  if (b < R)
    return cost / d;
  else
    return cost;
}

long 
sb_1(long cost, long R, long C, long b)
{
  long d;
  
  d = (long) ceil((b*1.0)/R);

  if (b >= R)
    return cost / d;
  else
    return cost;
}
#endif

#undef max
#define max(a,b) (a > b) ? a : b
#if 0
long
sa_1(long cost, long R, long C, long a)
{
  if (R >= (C / (1 << a)))
    return ceil((cost*1.0) / max((3-a),1));
  else if (R < (C /(1 << a)))
    return ceil(cost / max((2-a),1));
  else
    return cost;
}
#endif

#undef max


long
cm_count(Wht *W, long R, long C, long b, long a)
{
  int i, nn;
  long cm, N, Ni, Nt;
  Wht *Wi;

  N = W->N;

#if 0
  if (N <= ceil(C / R))
    return sb_1(N,R,C,b);
#endif

  if (N <= ceil(C*1.0/(a*1.0/R))*a)
    return sb_1(N,R,C,b);

  if (W->children == NULL) {
    if (R >= C)
      return 3*N;
    else /* R < C */
      return sb_1(2*N,R,C,b);
  } else {
    nn = W->children->nn;
    cm = 0;
    Nt = 1;

    for (i = 0; i < nn; i++) {
      Wi = W->children->Ws[i];
      Ni = Wi->N;
      /* cm += sb_2(N/Ni * cm_count(Wi, Nt*R, C, b, a), Nt*R, C, b); */
      cm += N/Ni * cm_count(Wi, Nt*R, C, b, a); 
      Nt *= Ni;
    }

    return cm;
  }
}
