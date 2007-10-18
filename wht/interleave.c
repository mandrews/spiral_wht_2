#include "wht.h"

bool
interleave_accept(Wht *W)
{
  if (W->attr[interleave_by] > W->right) {
    error_msg_set("interleave factor %d must be < %d in smallil(%d)[%d]", 
      W->attr[interleave_by],
      W->right,
      W->attr[interleave_by],
      W->n);
    return false;
  } 
    
  return true;
}


Wht *
interleave_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  size_t k;

  k = params[0];

  W            = small_init(name, n, params, np);
  W->accept    = interleave_accept;

  if (k > (1 << WHT_MAX_INTERLEAVE))
    wht_error("not configured for codelets of size %zd interleaved by %zd", n, k);

  W->attr[interleave_by] = k;

  return W;  
}

bool
split_interleave_accept(Wht *W)
{
  return split_accept(W);
}

void 
split_interleave_apply(Wht *W, long S, size_t D, wht_value *x)
{
  int nn;
  long N, R, S1, Ni, i, j, k;
  size_t nk;
  Wht *Wi;

  nn = W->children->nn;

  N  = W->N;
  R  = N;
  S1 = 1;


  for (i = 0; i < nn; i++) {
    Wi = W->children->Ws[i];
    Ni = W->children->ns[i];
    R /= Ni;

    if (Wi->attr[interleave_by] != -1)
      nk = Wi->attr[interleave_by];
    else
      nk = 1;

    for (j = 0; j < R; j++)
      for (k = 0; k < S1; k+=nk)
        (Wi->apply)(Wi, S1*S, S, x+k*S+j*Ni*S1*S);


    S1 *= Ni;
  }
}

Wht *
split_interleave_init(char *name, Wht *Ws[], size_t nn, int params[], size_t np) 
{
  Wht *W;

  W            = split_init(name, Ws, nn, params, np);
  W->apply     = split_interleave_apply;
  W->accept    = split_interleave_accept;

  return W;  
}

