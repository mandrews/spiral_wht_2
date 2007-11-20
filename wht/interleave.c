#include "wht.h"
#include "codelets.h"

void
small_interleave_transform(Wht *W)
{
  size_t k, k_max;

  k = W->params[0];

  k_max = (1 << WHT_MAX_INTERLEAVE);

  /* Check that codelet is small */
  if (W->children != NULL)
    return error_msg_set(W, "codelet must be small to be interleaved");

  /* Check that parent codelet is split interleaved */
  if ((W->parent == NULL) || (strcmp("splitil", W->parent->name) != 0)) 
    return error_msg_set(W, "codelet must be used in conjunction with splitil");

  /* Check that interleave factor is supported */
  if (k > k_max) 
    return error_msg_set(W, 
      "not configured for codelets of size %zd interleaved by %zd", W->n, k);

  /* Check it is possible to interleave */
  if (W->right <= 1) 
    return error_msg_set(W, "cannot be interleaved");

  /* Check that interleave factor is less than right I_n */
  if (k > W->right) 
    return error_msg_set(W, "interleave factor must be <= %d", W->right);

  W->apply = codelet_apply_lookup(W->n, "smallil", W->params, 1);

  if (W->apply == NULL) 
    return error_msg_set(W, "could not find codelet");

  W->attr[interleave_by] = k;
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

void
split_interleave_transform(Wht *W)
{
  /* Check that codelet is split */
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be interleaved");

  W->apply = split_interleave_apply;

  split_transform(W);
}

void
interleave_convert(Wht *W, int params[], size_t n)
{
  int i;
  size_t k_max;

  transform(W, "splitil", NULL, 0);

  k_max = (params[0] >> 1);

  for (i = k_max; i >= 1; i--) {
    int params[] = { (1 << i) };
    transform(W, "smallil", params, 1);
  }
}

