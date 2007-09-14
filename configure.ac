dnl Process this file with autoconf to produce a configure script.
AC_INIT(spiral_wht, 2.0, mjand@drexel.edu)

AM_INIT_AUTOMAKE(1.9)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
AC_DISABLE_STATIC
AM_DISABLE_STATIC

AC_CANONICAL_HOST

dnl
dnl M A I N T A I N E R   P A R A M E T E R S
dnl
MAX_UNROLL_PKG=8
MAX_INTERLEAVE_PKG=4
MAX_VECTOR_PKG=2

AC_SUBST(MAX_UNROLL_PKG)
AC_SUBST(MAX_INTERLEAVE_PKG)
AC_SUBST(MAX_VECTOR_PKG)



dnl
dnl D E F A U L T   P A R A M E T E R S
dnl
MAX_UNROLL=6
MAX_INTERLEAVE=4
VECTOR_SIZE=0



dnl
dnl T R A N S F O R M   T Y P E
dnl
AC_ARG_ENABLE([double], 
  [AC_HELP_STRING([--enable-double], 
    [compile with double precision floating point])], 
  [ok=$enableval], 
  [ok=yes])

if test "$ok" = "yes"; then
	AC_DEFINE(WHT_DOUBLE,1,
    [define to compile with double precision floating point])
  WHT_PRECISION=
  WHT_SUFFIX=
fi

AC_ARG_ENABLE([float], 
  [AC_HELP_STRING([--enable-float], 
    [compile wht with single precision floating point])], 
  [ok=$enableval], 
  [ok=no])

if test "$ok" = "yes"; then
	if test "$WHT_PRECISION" != ""; then
		AC_MSG_ERROR([--enable-float conflict])
	fi
	AC_DEFINE(WHT_FLOAT,1,
    [define to compile with single precision floating point])
	AC_DEFINE(WHT_DOUBLE,0)
  WHT_PRECISION=f
  WHT_SUFFIX=f
fi



dnl
dnl M A X    C O D E L E T    S I Z E
dnl
AC_ARG_ENABLE([max_unroll], 
  [AC_HELP_STRING([--enable-max-unroll=N], 
    [maximum unrolled codelet size])],
  [max_unroll=$enableval], 
  [max_unroll=$MAX_UNROLL])

if test $max_unroll -gt $MAX_UNROLL_PKG; then
  AC_MSG_ERROR([--enable-max-unroll larger than package was built for])
fi

AC_DEFINE_UNQUOTED(WHT_MAX_UNROLL,$max_unroll,
  [maximum unrolled codelet size])



dnl
dnl M A X   I N T E R L E A V E    F A C T O R 
dnl
AC_ARG_ENABLE([max_interleave], 
  [AC_HELP_STRING([--enable-interleave=K], 
    [interleave 2^K loop iterations in codelet])],
  [max_interleave=$enableval], 
  [max_interleave=$MAX_INTERLEAVE])

if test $max_interleave -gt $MAX_INTERLEAVE_PKG; then
  AC_MSG_ERROR([--enable-interleave larger than package was built for])
fi

AC_DEFINE_UNQUOTED(WHT_MAX_INTERLEAVE,$max_interleave,
  [interleave 2^K loop iterations in codelet])



dnl
dnl V E C T O R I Z A T I O N
dnl
AC_ARG_ENABLE([sse], 
  [AC_HELP_STRING([--enable-sse],
    [enable SSE optimizations])], 
  [have_sse=$enableval], 
  [have_sse=no])

if test "$have_sse" = "yes"; then
	AC_DEFINE(WHT_HAVE_SSE,1,
    [define to enable SSE optimizations.])

	have_simd="yes"
	if test "$WHT_PRECISION" != "f"; then
		AC_MSG_ERROR([--enable-sse can only be used in conjunction with --enable-float])
	fi

  VECTOR_SIZE=4
fi

AC_ARG_ENABLE([sse2], 
  [AC_HELP_STRING([--enable-sse2],
    [enable SSE2 optimizations])], 
  [have_sse2=$enableval], 
  [have_sse2=no])

if test "$have_sse2" = "yes"; then
	AC_DEFINE(WHT_HAVE_SSE2,1,[Define to enable SSE2 optimizations.])
	have_simd="yes"
	if test "$have_sse" = "yes"; then
		AC_MSG_ERROR([you cannot use SSE and SSE2 at the same time])
	fi
	if test "$WHT_PRECISION" != ""; then
		AC_MSG_ERROR([--enable-sse2 can only be used in conjunction with --enable-double])
	fi

  VECTOR_SIZE=2

fi
AC_DEFINE_UNQUOTED(WHT_VECTOR_SIZE,$VECTOR_SIZE,
  [Size of vector to use.])


dnl
dnl P A P I    M E A S U R E
dnl
AC_ARG_WITH([papi], 
  [AC_HELP_STRING([--with-papi=PATH],
    [compile with PAPI support, PATH is location of PAPI install])], 
  [papi_path=$withval], 
  [papi_path="no"])

AM_CONDITIONAL(PAPI_MEASURE, test "$papi_path" != "no")

PAPI_PATH=$papi_path
AC_SUBST(PAPI_PATH)

AC_ARG_ENABLE([cache-size-kb], 
  [AC_HELP_STRING([--enable-cache-size-kb],
    [size of L2 cache in KB])], 
  [cache_size_kb=$enableval], 
  [cache_size_kb=8])

CACHE_SIZE_KB=$cache_size_kb
AC_SUBST(CACHE_SIZE_KB)

dnl TODO not sure why this doesn't work
dnl if test "$enable_papi" = "yes"; then
dnl   AC_CHECK_HEADERS("$PAPI_PATH/include/papi.h",,
dnl     AC_MSG_ERROR([required header file for --enable-papi not found]))
dnl fi



dnl Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LEX
AC_PROG_YACC

dnl Check for library symbols
AC_CHECK_LIB(m, pow)

AC_CHECK_PROG(OCAMLC, ocamlc, ocamlc)
AC_CHECK_PROG(OCAMLOPT, ocamlopt, ocamlopt)
AC_CHECK_PROG(OCAMLDEP, ocamldep, ocamldep)
AC_CHECK_PROG(PERL,perl,perl)

AC_SUBST(OCAMLCFLAGS)
AC_SUBST(OCAMLOPTCFLAGS)
AC_SUBST(OCAMLDEFS)
AC_SUBST(OCAMLLDFLAGS)

AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_CHECK_HEADERS(math.h,,AC_MSG_ERROR([required header files not found]))

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

dnl add gcc warnings, in maintainer mode only
if test "$USE_MAINTAINER_MODE" = yes; then
  CFLAGS="$CFLAGS -Wall"
fi

AC_DEFINE_UNQUOTED(WHT_CC,"$CC",
  [the compiler used to compile this.])
AC_DEFINE_UNQUOTED(WHT_CFLAGS,"$CFLAGS",
  [the compiler flags used to compile this.])

AC_CONFIG_FILES([
   Makefile
   whtgen-ml/Makefile
   wht/Makefile
   wht/codelets/Makefile
   measure/Makefile
   measure/usec/Makefile
   measure/papi/Makefile
])

AC_OUTPUT
