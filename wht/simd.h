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
  R0 = _mm_add_pd(R1,R2)

#define vsub2(R0,R1,R2) \
  R0 = _mm_sub_pd(R1,R2)

#define vload2(R0,M0) \
  R0 = _mm_load_pd(&M0)

#define vstore2(R0,M0) \
  _mm_store_pd(&M0,R0)

#define vshuf2(R0,R1,R2,A,B) \
  R0 = _mm_shuffle_pd(R1, R2, _MM_SHUFFLE2(A,B))

#define vload2u(R0,A0,A1) \
  { \
    wht_vector2 TZ; \
    R0 = _mm_load_sd(&A0); \
    TZ = _mm_load_sd(&A1); \
    R0 = _mm_unpacklo_pd(R0,TZ); \
  };

#define vstore2u(R0,A0,A1) \
  _mm_storeh_pd(&A1,R0); \
  _mm_storel_pd(&A0,R0); 

#endif/*WHT_DOUBLE*/

#if (WHT_FLOAT == 1) && (WHT_HAVE_SSE == 1) && (WHT_VECTOR_SIZE == 4)
#include <xmmintrin.h>

typedef __m128 wht_vector4;

#define vadd4(R0,R1,R2) \
  R0 = _mm_add_ps(R1,R2)

#define vsub4(R0,R1,R2) \
  R0 = _mm_sub_ps(R1,R2)

#define vload4(R0,M0) \
  R0 = _mm_load_ps(&M0)

#define vstore4(R0,M0) \
  _mm_store_ps(&M0,R0)

#define vshuf4(R0,R1,R2,A,B,C,D) \
  R0 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(A,B,C,D))

#define vload4u(R0,A0,A1,A2,A3) \
  { \
    wht_vector4 R1; \
    R0 = _mm_load_ss(&A1); \
    R1 = _mm_load_ss(&A3); \
    R0 = _mm_loadh_pi(R0, (__m64 const *) &A0); \
    R1 = _mm_loadh_pi(R1, (__m64 const *) &A2); \
    vshuf4(R0,R0,R1,0,2,0,2); \
  };

#define vstore4u(R0,A0,A1,A2,A3) \
  { \
    _mm_store_ss(&A0,R0); \
    vshuf4(R0,R0,R0,0,3,2,1); \
    _mm_store_ss(&A1,R0); \
    vshuf4(R0,R0,R0,0,3,2,1); \
    _mm_store_ss(&A2,R0); \
    vshuf4(R0,R0,R0,0,3,2,1); \
    _mm_store_ss(&A3,R0); \
  };


#endif/*WHT_FLOAT*/


#endif/*SIMD_H*/
