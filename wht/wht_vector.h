#ifndef WHT_VECTOR_H
#define WHT_VECTOR_H

#if (WHT_DOUBLE == 1) && (WHT_HAVE_SSE2 == 1) && (WHT_VECTOR_SIZE == 2)
#include <emmintrin.h>

typedef __m128d wht_vector2;

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
  R0 = _mm_loadh_pd(R0,&A0); \
  R0 = _mm_loadl_pd(R0,&A1); 

#define vstore2u(R0,A0,A1) \
  _mm_storeh_pd(&A0,R0); \
  _mm_storel_pd(&A1,R0); 

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

#endif/*WHT_FLOAT*/


#endif/*WHT_VECTOR_H*/
