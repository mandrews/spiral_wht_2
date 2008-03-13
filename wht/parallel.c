/*
 * Copyright (c) 2007 Drexel University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/**
 * \file parallel.c
 * \author Kang Chen
 *
 * \brief Concurrent breakdown rules implementations.
 *
 */

#include "wht.h"
#include "registry.h"

#ifdef WHT_HAVE_OMP
#include <omp.h>
#endif/*WHT_HAVE_OMP*/

void split_parallel_apply(Wht *W, long S, size_t U, wht_value *x);
void split_ddl_parallel_apply(Wht *W, long S, size_t U, wht_value *x);

#define IL_ON

void
split_parallel_rule(Wht *W)
{
  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be parallelized");

  if (W->children->nn != 2)
    return error_msg_set(W, "codelet must be a binary split to be parallelized");

#ifdef IL_ON
	W->provides[INTERLEAVING] = true;
#endif
  W->apply = split_parallel_apply;
}

void
split_ddl_parallel_rule(Wht *W)
{
	long b;

  if (W->children == NULL)
    return error_msg_set(W, "codelet must be split to be parallelized");

  if (W->children->nn != 2)
    return error_msg_set(W, "codelet must be a binary split to be parallelized");
	
  b = W->rule->params[0];

  if (! i_power_of_2(b))
    return error_msg_set(W, "block size must be a power of 2");

  if (b > W->children->ns[0])
    return error_msg_set(W, "block size must be less than right child size %d", 
      W->children->ns[0]);

  if (b > W->children->ns[1])
    return error_msg_set(W, "block size must be less than left child size %d", 
      W->children->ns[1]);

  W->attr[BLOCK_SIZE] = b;

  W->apply = split_ddl_parallel_apply;
}


/* Tim Chagnon */
void 
m_bit_permute_parallel(long b, long M, long N, wht_value *x) 
{
  long NPB, PB, pb, NB, ib, jb, k, l, u, v;
  long ibN, jbN, kN;
  wht_value tmp;

  // M <= N must be true for this to work, Horizontal Pseudo-blocking
  NPB = N/M;  // number of pseudo-blocks
  PB = M;     // pseudo-block dimension
  //PBOFF = M;  // offset for each pseudo-block

  NB = PB/b;  // number of blocks

	#pragma omp single
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

void 
split_parallel_apply(Wht *W, long S, size_t U, wht_value *x)
{
#ifdef WHT_HAVE_OMP
  long block, chunk;
  long N, R, Ni, j;
  wht_value *xpt;
  long total, id;
	Wht *Wi;
#ifdef IL_ON
  int nIL;
#endif

  if (omp_in_parallel()) {
    N  = W->N;
    Ni = W->children->ns[0];
    R = W->children->ns[1];

    block = Ni * S;
		Wi = W->children->Ws[0];
    for (j = 0, xpt = x; j < R; ++ j, xpt += block)
			(Wi->apply)(Wi, S, S, xpt);

#ifdef IL_ON
		Wi = W->children->Ws[1];
		if (Wi->attr[INTERLEAVE_BY] == UNSET_ATTRIBUTE) {
      for (j = 0, xpt = x; j < Ni; ++ j, xpt += S)
				(Wi->apply)(Wi, S, S, xpt);
    } else {
      long temp;
      nIL = Wi->attr[INTERLEAVE_BY];
      temp = S * nIL;
      for (j = 0, xpt = x; j < Ni; j += nIL, xpt += temp)
				(Wi->apply)(Wi, block, S, xpt);
    }
#else
		Wi = W->children->Ws[1];
    for (j = 0, xpt = x; j < Ni; ++ j, xpt += S)
			(Wi->apply)(Wi, block, S, xpt);
#endif
  } 

  else {
#pragma omp parallel private (xpt, id, j, chunk) 
    { /* start of parallel region */
      N  = W->N;
      Ni = W->children->ns[0];
      R = W->children->ns[1];
      total = omp_get_num_threads();
      block = Ni * S;  

      /* no scheduling
      id = omp_get_thread_num();
      while (id < R) {
        xpt = x + id * block;  
        wht_apply(W->children->Ws[1], S, xpt);
        id += total;
      }*/

			Wi = W->children->Ws[0];
      chunk = R / total;
      if (chunk * total == R) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * block;
					(Wi->apply)(Wi, S, S, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= R - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * block;
						(Wi->apply)(Wi, S, S, xpt);
          }
        } else if (id < R) {
          for (; id < R; ++ id) {
            xpt = x + id * block;
						(Wi->apply)(Wi, S, S, xpt);
          }
        }
      }

#pragma omp barrier
#ifdef IL_ON
			Wi = W->children->Ws[1];
			nIL = Wi->attr[INTERLEAVE_BY];
			if (Wi->attr[INTERLEAVE_BY] == UNSET_ATTRIBUTE) {
        /* no scheduling
        id = omp_get_thread_num();
        while (id < Ni) {
          xpt = x + (int) (id * S);  
          wht_apply(W->children->Ws[0], block, xpt);
          id += total;
        }*/

        chunk = Ni / total;
        if (chunk * total == Ni) {
          /* each thread has an equal share of work */
          id = omp_get_thread_num() * chunk;
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * S;
						(Wi->apply)(Wi, block, S, xpt);
          }
        } else {
          /* some threads have more work to do */      	
          ++ chunk;
          id = omp_get_thread_num() * chunk;
          if (id <= Ni - chunk) {
            for (j = 0; j < chunk; ++ j, ++ id) {
              xpt = x + id * S;
							(Wi->apply)(Wi, block, S, xpt);
            }
          } else if (id < Ni) {
            for (; id < Ni; ++ id) {
              xpt = x + id * S;
							(Wi->apply)(Wi, block, S, xpt);
            }
          } else {
            /* idle */
          }
        }

      } else {
				nIL = Wi->attr[INTERLEAVE_BY];
        if (Ni < total * nIL) {
          /* no scheduling */
          id = omp_get_thread_num() * nIL;
          while (id < Ni) {
            xpt = x + id * S;  
						(Wi->apply)(Wi, block, S, xpt);
            id += total * nIL;
          }

        } else {
          chunk = Ni / total;
          if (chunk * total == Ni) {
            id = omp_get_thread_num() * chunk;
            for (j = 0; j < chunk; j += nIL, id += nIL) {
              xpt = x + id * S;
							(Wi->apply)(Wi, block, S, xpt);
            }
          } else {
            chunk = (Ni / (total * nIL)) * nIL;
            chunk += nIL;
            id = omp_get_thread_num() * chunk;
            if (id <= Ni - chunk) {
              for (j = 0; j < chunk; j += nIL, id += nIL) {
                xpt = x + id * S;
								(Wi->apply)(Wi, block, S, xpt);
              }
            } else if (id < Ni) {
              for (; id < Ni; id += nIL) {
                xpt = x + id * S;
								(Wi->apply)(Wi, block, S, xpt);
              }
            }
          }
        }
      }
#else
			Wi = W->children->Ws[1];
      chunk = Ni / total;
      if (chunk * total == Ni) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * S;
					(Wi->apply)(Wi, block, S, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= Ni - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * S;
						(Wi->apply)(Wi, block, S, xpt);
          }
        } else if (id < Ni) {
          for (; id < Ni; ++ id) {
            xpt = x + id * S;
						(Wi->apply)(Wi, block, S, xpt);
          }
        }
      }
#endif
    } /*end of parallel region*/
  }
#else
  wht_exit("initialization guards should prevent this message");
#endif
}

void 
split_ddl_parallel_apply(Wht *W, long S, size_t U, wht_value *x)
{
  long block, chunk;
  long N, R, Ni, j;
  wht_value *xpt;
  long total, id;
  int left=0, right=1;
	Wht *Wi;
	size_t b;

  if (W->children->ns[1] < W->children->ns[0]) {
    left = 1;
    right = 0;
  }

  b = W->attr[BLOCK_SIZE];

  if (omp_in_parallel()) {
    N  = W->N;
    Ni = W->children->ns[right];
    R = W->children->ns[left];

		Wi = W->children->Ws[right];
    block = Ni * S;
    for (j = 0, xpt = x; j < R; ++ j, xpt += block)
			(Wi->apply)(Wi, S, S, xpt);

			// (Wi->apply)(Wi, S*R, S, xpt);

    /* with DDL */
    //p_reorganization(x, N, R, S, 0);
		m_bit_permute_parallel(b, R, Ni, x);

    block = R * S;
		Wi = W->children->Ws[left];
    for (j = 0, xpt = x; j < Ni; ++ j, xpt += block)
			(Wi->apply)(Wi, S, S, xpt);
    //p_reorganization(x, N, R, S, 0);
		
		m_bit_permute_parallel(b, R, Ni, x);
    
  } 
  else {
#pragma omp parallel private (xpt, id, chunk, j) 
    { 
      N  = W->N;
      Ni = W->children->ns[right];
      R = W->children->ns[left];
      block = Ni * S;  
      total = omp_get_num_threads();

      /* no scheduling
      id = omp_get_thread_num();
      while (id < R) {
        xpt = x + id * block;  
        wht_apply(W->children->Ws[right], S, xpt);
        id += total;
      }*/
			Wi = W->children->Ws[right];

      chunk = R / total;
      if (chunk * total == R) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * block;
					(Wi->apply)(Wi, S, S, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= R - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * block;
						(Wi->apply)(Wi, S, S, xpt);
          }
        } else if (id < R) {
          for (; id < R; ++ id) {
            xpt = x + id * block;
						(Wi->apply)(Wi, S, S, xpt);
          }
        }
      }


#pragma omp barrier
			m_bit_permute_parallel(b, R, Ni, x);
  
#pragma omp barrier
      block = R * S;
      /* no scheduling
      id = omp_get_thread_num();
      while (id < Ni) {
        xpt = x + id * block;  
        wht_apply(W->children->Ws[left], S, xpt);
        id += total;
      }*/

      chunk = Ni / total;
			Wi = W->children->Ws[left];
      if (chunk * total == Ni) {
        id = omp_get_thread_num() * chunk;
        for (j = 0; j < chunk; ++ j, ++ id) {
          xpt = x + id * block;
					(Wi->apply)(Wi, S, S, xpt);
        }
      } else {
        ++ chunk;
        id = omp_get_thread_num() * chunk;
        if (id <= Ni - chunk) {
          for (j = 0; j < chunk; ++ j, ++ id) {
            xpt = x + id * block;
						(Wi->apply)(Wi, S, S, xpt);
          }
        } else if (id < Ni) {
          for (; id < Ni; ++ id) {
            xpt = x + id * block;
						(Wi->apply)(Wi, S, S, xpt);
          }
        }
      }

#pragma omp barrier
			m_bit_permute_parallel(b, R, Ni, x);
     
    } /*end of parallel region*/
  }
}

