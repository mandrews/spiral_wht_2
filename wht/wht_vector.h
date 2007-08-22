#ifndef WHT_VECTOR_H
#define WHT_VECTOR_H

#ifdef __GNUC__

typedef int v2si __attribute__ ((vector_size(8))); /* size in bytes */

#if PRECISION == d
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

#endif/*PRECISION*/

#endif/*__GNUC__*/

#endif/*WHT_VECTOR_H*/
