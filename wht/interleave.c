#include "wht.h"
#include "codelets.h"

/** \todo Lookup based on string, not params list */
bool
small_interleave_transform(Wht *W)
{
  size_t k, k_max;

  k = W->params[0];

  k_max = (1 << WHT_MAX_INTERLEAVE);

  /* Check that parent codelet is split interleaved */
  if ((W->parent == NULL) || (strcmp("splitil", W->parent->name) != 0)) {
    erro_msg_set("codelet %s must be used in conjunction with splitil", 
      W->to_string(W));
    return false;
  }

  /* Check that interleave factor is supported */
  if (k > k_max) {
    erro_msg_set("not configured for codelets of size %zd interleaved by %zd", W->n, k);
    return false;
  }

  /* Check that interleave factor is less than right I_n */
  if (k > W->right) {
    erro_msg_set("interleave factor %d must be < %d in %s", 
      W->to_string(W), k, W->right);
    return false;
  }

  W->apply = codelet_apply_lookup(W->n, "smallil", W->params, 1);

  if (W->apply == NULL) {
    erro_msg_set("could not find codelet %s", W->to_string(W));
    return false;
  }

  W->attr[interleave_by] = k;

  return true;
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

    if (Wi->attr[interleave_by] != UNSET_ATTRIBUTE)
      nk = Wi->attr[interleave_by];
    else
      nk = 1;

    for (j = 0; j < R; j++)
      for (k = 0; k < S1; k+=nk)
        (Wi->apply)(Wi, S1*S, S, x+k*S+j*Ni*S1*S);


    S1 *= Ni;
  }
}

bool
split_interleave_transform(Wht *W)
{
  W->apply = split_interleave_apply;

  return split_transform(W);
}

#if 0
Wht *
small_interleave_convert(Wht *W, size_t k)
{
  size_t kp, k_min;
  int *params;

  if (W->right < (1 << WHT_MAX_INTERLEAVE))
    k_min = W->right;
  else
    k_min = (1 << WHT_MAX_INTERLEAVE);
 
  for (kp = k; kp > k_min; kp--);

  if (kp < 2) 
    return small_init("small", W->n, NULL, 0);

  params = i_malloc(sizeof(*params)*1);
  params[0] = kp;

  return interleave_init("smallil", W->n, params, 1);
}

Wht *
split_interleave_convert(Wht *W, size_t k)
{
  size_t nn, i;
  Wht **Ws;

  nn = W->children->nn;
  Ws = i_malloc(sizeof(*Ws) *nn);

  for (i = 0; i < nn; i++) 
    Ws[i] = interleave_convert(W->children->Ws[i], k);

  return split_interleave_init("splitil", Ws, nn, NULL, 0);
}

Wht *
interleave_convert(Wht *W, size_t k)
{
  if (W->children)
    return split_interleave_convert(W, k);
  else
    return small_interleave_convert(W, k);
}
#endif

