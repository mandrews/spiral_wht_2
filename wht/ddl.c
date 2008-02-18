/**
 * \file splitddl.c
 *
 * \brief Implementation of methods for binary split with "Dynamic Data Layout"
 */


#include "wht.h"

/* Tim Chagnon */
void m_bit_permute(long b, long M, long N, wht_value *x) {
  long NPB, PB, PBOFF, pb, NB, ib, jb, k, l, lstart, u, v;
  long ibN, jbN, kN;
  wht_value tmp;

  // M <= N must be true for this to work, Horizontal Pseudo-blocking
  NPB = N/M;  // number of pseudo-blocks
  PB = M;     // pseudo-block dimension
  //PBOFF = M;  // offset for each pseudo-block

  NB = PB/b;  // number of blocks

  /* orig */ /*
  for (pb=0; pb<NPB; pb++) {
    for (i=0; i<NB; i++) {
      for (j=i; j<NB; j++) {
        for (k=0; k<b; k++) {
          // If diag block, start above
          lstart = (j==i) ? k+1 : 0;
          for (l=lstart; l<b; l++) {
            u = (i*b+k)*N + (j*b+l) + pb*PBOFF;
            v = (j*b+l)*N + (i*b+k) + pb*PBOFF;
            tmp = x[u];
            x[u] = x[v];
            x[v] = tmp;
          }
        }
      }
    }
  }
  */

  for (pb=0; pb<N; pb+=PB) {
    for (ib=0; ib<PB; ib+=b) {
      ibN = ib*N;
      // end diag
      for (jb=ib+b; jb<PB; jb+=b) {
        jbN = jb*N;
        for (k=0; k<b; k++) {
          kN = k*N;
          for (l=0; l<b; l++) {
            u = ibN + kN + jb + l + pb;
            v = jbN + l*N + ib + k + pb;
            tmp = x[u];
            x[u] = x[v];
            x[v] = tmp;
          }
        }
      }
      // diag blocks
      //jb = ib;
      //jbN = jb*N;
      for (k=0; k<b; k++) {
        kN = k*N;
        // If diag block, start above
        for (l=k+1; l<b; l++) {
          u = ibN + kN + ib + l + pb;
          v = ibN + l*N + ib + k + pb;
          tmp = x[u];
          x[u] = x[v];
          x[v] = tmp;
        }
      }
    }
  }
}

/* 
 *  To improve cache hit ratio during application of a child WHT, stride N
 *  access (W_M x I_N) can be transformed to N stride 1 accesses by permuting
 *  the input signal before and after application.
 *
 *  As implemented for a binary split:
 *    WHT_N = (W_N1 x I_N2)(I_N1 x W_N2)
 *  Is implemented here as:
 *          = L(N1*N2, N1) (I_N2 x W_N1) L(N1*N2, N2) (I_N1 x W_N2)
 *  Where L(m*n, n) is a permutation matrix of size m*n by m*n with stride n
 *
 *  L(m*n, n) can be computed in trivial form by the 2-loop:
 *    for i from 0 to m-1
 *      for j from 0 to n-1
 *        y[j*m+i] = x[i*n+j]
 *
 *  A blocking form is required for better cache performance.
 *
*/
void 
split_ddl_apply(Wht *W, long S, size_t D, wht_value *x)
{
  int nn;
  long N, N1, N2, i, b, r, s;

  nn = W->children->nn;

  N  = W->N;

  // blocksize
  b = W->rule->params[0];

  // Children are stored in Right-to-Left order
  N1 = W->children->ns[1];
  N2 = W->children->ns[0];

  // (I_N1 x W_N2)
  for (i=0; i<N1; i++) {
    (W->children->Ws[0]->apply)(W->children->Ws[0], S, 1, x+i*N2);
  }

  // S > R
  if (N2 > N1) {
    r = N1;
    s = N2;
  } else {
    r = N2;
    s = N1;
  }

  // Pseudo-transpose at S
  m_bit_permute(b, r, s, x);

  // (I_N2 x W_N1)
  for (i=0; i<N2; i++) {
    (W->children->Ws[1]->apply)(W->children->Ws[1], S, 1, x+i*N1);
  }

  // Pseudo-transpose at S
  m_bit_permute(b, r, s, x);

}

/**
 * \brief Replace split node with ddl split node if possible.
 *
 * \param   W   WHT Plan
 */
void
split_ddl_rule(Wht *W)
{
  long b, bp, i;

  /* Check that codelet is split */
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split for ddl");

  if (W->children->nn != 2)
    return error_msg_set(W, "codelet must be binary split for ddl");

  b = W->rule->params[0];

  if (! i_power_of_2(b))
    return error_msg_set(W, "block size must be a power of 2");

  if (b > W->children->ns[0])
    return error_msg_set(W, "block size must be less than right child size %d", 
      W->children->ns[0]);

  if (b > W->children->ns[1])
    return error_msg_set(W, "block size must be less than left child size %d", 
      W->children->ns[1]);

  W->apply = split_ddl_apply;
}

