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
 * \file simd.h
 * \author Michael Andrews
 *
 * \brief Single Instruction Multiple Data (SIMD) macro interface file.
 *
 * This file is included by simd vector codelets, and uses Intel SSE and SSE2
 * intrinsics (x86 wrapper macros).
 *
 * \todo Implement Altivec interfaces.
 */
#ifndef SIMD_H
#define SIMD_H

#if (WHT_DOUBLE == 1) && (WHT_HAVE_SSE2 == 1) && (WHT_VECTOR_SIZE == 2)
#include <emmintrin.h>

typedef __m128d wht_vector2;

/**
 * \brief Vector of size 2 Add
 *
 * \param	R0		R1 + R2
 * \param R1  	Input
 * \param R2  	Input
 */
#define vadd2(R0,R1,R2) \
  R0 = _mm_add_pd(R1,R2);

#define vsub2(R0,R1,R2) \
  R0 = _mm_sub_pd(R1,R2);

#define __vload2(R0,M0) \
  R0 = _mm_load_pd(&M0);



#ifndef TRACE
#define vload2(R0,M0) __vload2(R0,M0); 
#else
#define vload2(R0,M0) __vload2(R0,M0); \
	fprintf(stderr, "0 %p\n", &M0); 
#endif/*TRACE*/



#define __vstore2(R0,M0) \
  _mm_store_pd(&M0,R0);

#ifndef TRACE
#define vstore2(R0,M0) __vstore2(R0,M0); 
#else
#define vstore2(R0,M0) __vstore2(R0,M0); \
	fprintf(stderr, "1 %p\n", &M0); 
#endif/*TRACE*/



#define vshuf2(R0,R1,R2,A,B) \
  R0 = _mm_shuffle_pd(R1, R2, _MM_SHUFFLE2(A,B));



#define __vload2u(R0,A0,A1) \
  { \
    wht_vector2 TZ; \
    R0 = _mm_load_sd(&A0); \
    TZ = _mm_load_sd(&A1); \
    R0 = _mm_unpacklo_pd(R0,TZ); \
  };

#ifndef TRACE
#define vload2u(R0,A0,A1) __vload2u(R0,A0,A1); 
#else
#define vload2u(R0,A0,A1) __vload2u(R0,A0,A1); \
	fprintf(stderr, "0 %p\n", &A0); \
	fprintf(stderr, "0 %p\n", &A1);
#endif/*TRACE*/



#define __vstore2u(R0,A0,A1) \
  _mm_storeh_pd(&A1,R0); \
  _mm_storel_pd(&A0,R0); 

#ifndef TRACE
#define vstore2u(R0,A0,A1) __vstore2u(R0,A0,A1); 
#else
#define vstore2u(R0,A0,A1) __vstore2u(R0,A0,A1); \
	fprintf(stderr, "1 %p\n", &A1); \
	fprintf(stderr, "1 %p\n", &A0);
#endif/*TRACE*/

#endif/*WHT_DOUBLE*/

#if (WHT_FLOAT == 1) && (WHT_HAVE_SSE == 1) && (WHT_VECTOR_SIZE == 4)
#include <xmmintrin.h>

typedef __m128 wht_vector4;

#define vadd4(R0,R1,R2) \
  R0 = _mm_add_ps(R1,R2);

#define vsub4(R0,R1,R2) \
  R0 = _mm_sub_ps(R1,R2);



#define __vload4(R0,M0) \
  R0 = _mm_load_ps(&M0);

#ifndef TRACE
#define vload4(R0,M0) __vload4(R0,M0);
#else
#define vload4(R0,M0) __vload4(R0,M0); \
  fprintf(stderr, "0 %p\n", &M0);
#endif/*TRACE*/



#define __vstore4(R0,M0) \
  _mm_store_ps(&M0,R0);

#ifndef TRACE
#define vstore4(R0,M0) __vstore4(R0,M0);
#else
#define vstore4(R0,M0) __vstore4(R0,M0); \
  fprintf(stderr, "1 %p\n", &M0);
#endif/*TRACE*/



#define vshuf4(R0,R1,R2,A,B,C,D) \
  R0 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(A,B,C,D));



#define __vload4u(R0,A0,A1,A2,A3) \
  { \
    wht_vector4 R1; \
    R0 = _mm_load_ss(&A1); \
    R1 = _mm_load_ss(&A3); \
    R0 = _mm_loadh_pi(R0, (__m64 const *) &A0); \
    R1 = _mm_loadh_pi(R1, (__m64 const *) &A2); \
    vshuf4(R0,R0,R1,0,2,0,2); \
  };

#ifndef TRACE
#define vload4u(R0,A0,A1,A2,A3) __vload4u(R0,A0,A1,A2,A3);
#else
#define vload4u(R0,A0,A1,A2,A3) __vload4u(R0,A0,A1,A2,A3); \
  fprintf(stderr, "0 %p\n", &A1); \
  fprintf(stderr, "0 %p\n", &A3); \
  fprintf(stderr, "0 %p\n", &A0); \
  fprintf(stderr, "0 %p\n", &A2);
#endif/*TRACE*/




#define __vstore4u(R0,A0,A1,A2,A3) \
  { \
    _mm_store_ss(&A0,R0); \
    vshuf4(R0,R0,R0,0,3,2,1); \
    _mm_store_ss(&A1,R0); \
    vshuf4(R0,R0,R0,0,3,2,1); \
    _mm_store_ss(&A2,R0); \
    vshuf4(R0,R0,R0,0,3,2,1); \
    _mm_store_ss(&A3,R0); \
  };

#ifndef TRACE
#define vstore4u(R0,A0,A1,A2,A3) __vstore4u(R0,A0,A1,A2,A3);
#else
#define vstore4u(R0,A0,A1,A2,A3) __vstore4u(R0,A0,A1,A2,A3); \
  fprintf(stderr, "1 %p\n", &A0); \
  fprintf(stderr, "1 %p\n", &A1); \
  fprintf(stderr, "1 %p\n", &A2); \
  fprintf(stderr, "1 %p\n", &A3);
#endif/*TRACE*/



#endif/*WHT_FLOAT*/


#endif/*SIMD_H*/
