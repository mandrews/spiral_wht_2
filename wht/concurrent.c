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

void 
split_tmp_apply(Wht *W, long S, size_t U, wht_value *x) 
{
  long block, chunk;
  long N, R, S1, Ni, j;
  wht_value *xpt;
  long total, id;

  if (omp_in_parallel()) {
    N  = W->N;
    Ni = W->children->ns[1];
    R = W->children->ns[0];

    block = Ni * S;
    for (j = 0, xpt = x; j < R; ++ j, xpt += block)
      (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);

    for (j = 0, xpt = x; j < Ni; ++ j, xpt += S)
      (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
  } 

  else {
#pragma omp parallel private (xpt, id, j, chunk) 
    { /* start of parallel region */
      N  = W->N;
      Ni = W->children->ns[1];
      R = W->children->ns[0];
      total = omp_get_num_threads();
      block = Ni * S;  

      /* no scheduling
      id = omp_get_thread_num();
      while (id < R) {
        xpt = x + id * block;  
        (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
        id += total;
      }*/

      chunk = R / total;
      if (chunk * total == R) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * block;
          (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= R - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * block;
            (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
          }
        } else if (id < R) {
          for (; id < R; ++ id) {
            xpt = x + id * block;
            (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, xpt);
          }
        }
      }

#pragma omp barrier
      chunk = Ni / total;
      if (chunk * total == Ni) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * S;
          (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= Ni - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * S;
            (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
          }
        } else if (id < Ni) {
          for (; id < Ni; ++ id) {
            xpt = x + id * S;
            (W->children->Ws[0]->apply)(W->children->Ws[0], block, 1, xpt);
          }
        }
      }
    } /*end of parallel region*/
  }
}

