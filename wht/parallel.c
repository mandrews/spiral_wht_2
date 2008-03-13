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

	/* p_transpose requires this */
  if (b <= 2)
    return error_msg_set(W, "block size must be greater than 2");

#if 0
	/* m_bit_permute requires this */
  if (b > W->children->ns[0])
    return error_msg_set(W, "block size must be less than right child size %d", 
      W->children->ns[0]);

  if (b > W->children->ns[1])
    return error_msg_set(W, "block size must be less than left child size %d", 
      W->children->ns[1]);
#endif

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

#define BlockSize 32
#define NoUnroll 4

void 
p_transpose(wht_value *xx, int n, int n1, int pll, int B)
{
  wht_value **x;
	int Bi, Bj;
  int i, j, k, xbi, xbj;
  int band_id = 0, load, beta, shift, totalload, row = 0, col = 0;
  wht_value temp0, temp1, temp2, temp3;
  int n2 = n / n1;
  int id, total;

  if (n1 < B) {
    B = n1;
    beta = 1;
  } else {
    beta = n1 / B;
  }

  if (pll) {
#ifdef _OPENMP
    total = omp_get_num_threads();
    id = omp_get_thread_num();
#else
    total = 1;
    id = 0;
#endif
  } else {
    total = 1;
    id = 0;
  }

  x = (wht_value **) malloc(n1 * sizeof(wht_value *));
  
  /* determine total blocks (work) for each thread */
  totalload = beta * (beta + 1) / 2;
  shift = totalload % total;
  (shift) ? (load = totalload / total + 1) : (load = totalload / total);

  /*fprintf(stderr,"id%d beta%d load%d blocksize%d\n",id, beta, load, B);*/
  
  while (band_id < n2) {
    x[0] = &(xx[0]) + band_id;
    band_id += n1;

    for (i = 1; i < n1; ++ i) 
      x[i] = x[i-1] + n2;

    col = load * id;
    if (col >= totalload) {
      id = (id + total - shift) % total;
      continue;
    }
    row = 0;
    temp0 = beta;
    while (col >= temp0) {
      col -= temp0;
      temp0 --;
      row ++;
    }
    xbj = row * B;
    xbi = (col + row) * B;

    for (k = 0; k < load; ++ k) {
      if (xbj >= n1) break;
      /*fprintf(stderr, "id%d load%d band%d n1_%d n2_%d xbi%d xbj%d\n", 
        id, load, band_id, n1, n2, xbi, xbj);*/
      Bi = B + xbi;
      Bj = B + xbj;
      if (xbi == xbj) {
        for(i = xbi; i< Bi; ++ i) {
          for (j = i + 1; j < Bj; ++ j) {
            temp0 = x[i][j];
            x[i][j] =  x[j][i];
            x[j][i] = temp0;
          }
        }
      } else {
        for(j = xbj; j < Bj; ++ j) {
          for(i = xbi; i < Bi; i += NoUnroll) {
            temp0 = x[i][j];
            temp1 = x[i+1][j];
            temp2 = x[i+2][j];
            temp3 = x[i+3][j]; 
                      
            x[i][j]   = x[j][i];
            x[i+1][j] = x[j][i+1];
            x[i+2][j] = x[j][i+2];
            x[i+3][j] = x[j][i+3]; 
                      
            x[j][i]  = temp0;
            x[j][i+1]= temp1;
            x[j][i+2]= temp2;
            x[j][i+3]= temp3; 
          }
        }
      }
      xbi += B;
      if (xbi >= n1) {
        xbj += B;
	xbi = xbj;
      }
    } 
    id = (id + total - shift) % total;
  }
  free(x);
}

void 
p_transpose_stride(wht_value *xx, int n, int n1, long S, int pll, int B)
{
  wht_value **x;
  int i, j, ii, jj, xbi, xbj;
  wht_value temp0, temp1, temp2, temp3;
  int n2 = n * S / n1;
  int id, total;

  /*#pragma omp parallel shared(xx)*/
  {
  x = (wht_value **) malloc(n1*sizeof(wht_value *));
  if (pll) {
#ifdef _OPENMP    
    total = omp_get_num_threads() * n1 * S;
    id = omp_get_thread_num() * n1 * S;  
#else
    total = n1 * S;
    id = 0;
#endif
  } else {
    total = n1 * S;
    id = 0;
  }
  
  if (n1 < B) 
    B = n1;

  while (id < n2) {
    x[0] = &(xx[0]) + id;
    id += total;
    
    for(i = 1; i < n1; ++ i) 
      x[i] = x[i-1] + n2;

    for(xbj = 0; xbj < n1; xbj += B) {
      xbi = xbj;
      for(i = xbi; i < B + xbi; ++ i) {
        for(j = i + 1; j < B + xbj; ++ j) {
          temp0 = x[i][j * S];
          x[i][j * S] = x[j][i * S];
          x[j][i * S] = temp0;
        }
      }
      xbi += B;
      for( ; xbi < n1; xbi += B) {
        int jjs = xbj * S;
        j = xbj + B;
        for(jj = xbj; jj < j; ++ jj) {
          int iis = xbi * S;
          jjs += S;
          i = xbi + B;
          for(ii = xbi; ii < i; ii += NoUnroll) {
            iis += S;
            
            temp0 = x[ii][jjs];
            temp1 = x[ii + 1][jjs];
            temp2 = x[ii + 2][jjs];
            temp3 = x[ii + 3][jjs];
                      
            x[ii][jjs]   = x[jj][iis];
            x[ii+1][jjs] = x[jj][iis + S];
            x[ii+2][jjs] = x[jj][iis + 2*S];
            x[ii+3][jjs] = x[jj][iis + 3*S];
                      
            x[jj][iis]      = temp0;
            x[jj][iis + S]  = temp1;
            x[jj][iis + 2*S]= temp2;
            x[jj][iis + 3*S]= temp3;
          }
        }
      }
    }
  }
  free(x);
  
  } /*end of omp pragma*/
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
				(Wi->apply)(Wi, block, S, xpt);
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

#define p_reorganization(x,n,n1,s,p,b) \
if ((s) == 1) {         \
  p_transpose(x,n,n1,p,b);    \
} else {                \
  p_transpose_stride(x,n,n1,s,p,b); \
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
    p_reorganization(x, N, R, S, 0, b);
		//p_transpose(x,N,R,S);
		//m_bit_permute_parallel(b, R, Ni, x);

    block = R * S;
		Wi = W->children->Ws[left];
    for (j = 0, xpt = x; j < Ni; ++ j, xpt += block)
			(Wi->apply)(Wi, S, S, xpt);

    p_reorganization(x, N, R, S, 0, b);
		// m_bit_permute_parallel(b, R, Ni, x);
		// p_transpose(x,N,R,S);
    
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
			//m_bit_permute_parallel(b, R, Ni, x);
      p_reorganization(x, N, R, S, 1, b);
  
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
			// m_bit_permute_parallel(b, R, Ni, x);
      p_reorganization(x, N, R, S, 1, b);
     
    } /*end of parallel region*/
  }
}

