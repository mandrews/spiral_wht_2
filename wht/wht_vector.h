#ifndef WHT_VECTOR_H
#define WHT_VECTOR_H

#ifdef WHT_DOUBLE 
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

#endif/*WHT_DOUBLE*/

#ifdef WHT_FLOAT 
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
  R0 = _mm_shuffle_ps(R1, R2, MM_SHUFFLE(A,B,C,D))

#endif/*WHT_FLOAT*/


#endif/*WHT_VECTOR_H*/
