#ifndef WHT_VECTOR_H
#define WHT_VECTOR_H

#ifdef __GNUC__

typedef int v2si __attribute__ ((vector_size(8))); /* size in bytes */

#ifdef WHT_DOUBLE 
typedef double v2df __attribute__ ((vector_size(16))); /* size in bytes */

typedef v2df wht_vector2;

/* 
 * From: gcc/x86_64-pc-linux-gnu/3.4.4/include/emmintrin.h
 */

#define vadd2(R0,R1,R2) \
  R0 = __builtin_ia32_addpd(R1,R2)

#define vsub2(R0,R1,R2) \
  R0 = __builtin_ia32_subpd(R1,R2)

#define vload2(R0,M0) \
  R0 = __builtin_ia32_loadapd(&M0)

#define vstore2(R0,M0) \
  __builtin_ia32_storeapd(&M0,R0)

#define MM_SHUFFLE2(R1,R0) \
 (((R1) << 1) | (R0))

#define vshuf2(R0,R1,R2,A,B) \
  R0 = __builtin_ia32_shufpd(R1, R2, MM_SHUFFLE2(A,B))

#endif/*WHT_DOUBLE*/

#ifdef WHT_FLOAT 
typedef float v4sf __attribute__ ((vector_size(16))); /* size in bytes */

typedef v4sf wht_vector4;

/* 
 * From: gcc/x86_64-pc-linux-gnu/3.4.4/include/emmintrin.h
 */

#define vadd4(R0,R1,R2) \
  R0 = __builtin_ia32_addps(R1,R2)

#define vsub4(R0,R1,R2) \
  R0 = __builtin_ia32_subps(R1,R2)

#define vload4(R0,M0) \
  R0 = __builtin_ia32_loadaps(&M0)

#define vstore4(R0,M0) \
  __builtin_ia32_storeaps(&M0,R0)

#define MM_SHUFFLE4(R3,R2,R1,R0) \
 (((R3) << 6) | ((R2) << 4) | ((R1) << 2) | (R0))

#define vshuf4(R0,R1,R2,A,B,C,D) \
  R0 = __builtin_ia32_shufps(R1, R2, MM_SHUFFLE4(A,B,C,D))

#endif/*WHT_FLOAT*/


#endif/*__GNUC__*/

#endif/*WHT_VECTOR_H*/
