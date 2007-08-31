dnl Process this file with autoconf to produce a configure script.
AC_INIT(spiral_wht, 2.0, mjand@drexel.edu)

AM_INIT_AUTOMAKE(1.9)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
AC_DISABLE_STATIC
AM_DISABLE_STATIC

AC_CANONICAL_HOST

dnl Double precision floating point
AC_ARG_ENABLE(
  double, 
  [AC_HELP_STRING([--enable-double], [compile with double precision floating point])], 
  ok=$enableval, ok=yes)

if test "$ok" = "yes"; then
	AC_DEFINE(WHT_DOUBLE,1,[Define to compile with double precision floating point.])
  WHT_PRECISION=
  WHT_SUFFIX=
fi

dnl Single precision floating point
AC_ARG_ENABLE(
  float, 
  [AC_HELP_STRING([--enable-float], [compile wht with single precision floating point])], 
  ok=$enableval, ok=no)

if test "$ok" = "yes"; then
	if test "$WHT_PRECISION" != ""; then
		AC_MSG_ERROR([--enable-float conflict])
	fi
	AC_DEFINE(WHT_FLOAT,1,[Define to compile with single precision floating point.])
	AC_DEFINE(WHT_DOUBLE,0)
  WHT_PRECISION=f
  WHT_SUFFIX=f
fi

dnl
dnl V E C T O R I Z A T I O N
dnl
WHT_VECTOR_SIZE=0
AC_ARG_ENABLE(sse, 
  [AC_HELP_STRING([--enable-sse],[enable SSE optimizations])], 
  have_sse=$enableval, have_sse=no)

if test "$have_sse" = "yes"; then
	AC_DEFINE(HAVE_SSE,1,[Define to enable SSE optimizations.])
	have_simd="yes"
	if test "$WHT_PRECISION" != "f"; then
		AC_MSG_ERROR([--enable-sse can only be used in conjunction with --enable-float])
	fi
  AC_DEFINE(WHT_VECTOR_SIZE,4,[Size of vector to use.])
fi

AC_ARG_ENABLE(sse2, 
  [AC_HELP_STRING([--enable-sse2],[enable SSE2 optimizations])], 
  have_sse2=$enableval, have_sse2=no)

if test "$have_sse2" = "yes"; then
	AC_DEFINE(HAVE_SSE2,1,[Define to enable SSE2 optimizations.])
	have_simd="yes"
	if test "$have_sse" = "yes"; then
		AC_MSG_ERROR([you cannot use SSE and SSE2 at the same time])
	fi
	if test "$WHT_PRECISION" != ""; then
		AC_MSG_ERROR([--enable-sse2 can only be used in conjunction with --enable-double])
	fi
  AC_DEFINE(WHT_VECTOR_SIZE,2,[Size of vector to use.])
fi
dnl
dnl V E C T O R I Z A T I O N
dnl


dnl PAPI path for measure
AC_ARG_WITH(papi-path, 
  [AC_HELP_STRING([--with-papi-path],[Location of PAPI install])], 
  papi_path=$withval, papi_path="/usr")

AC_ARG_WITH(cache-size-kb, 
  [AC_HELP_STRING([--with-cache-size-kb],[Size of L2 cache in KB])], 
  cache_size_kb=$withval, cache_size_kb=8)

AC_SUBST(papi_path)

CACHE_SIZE_KB=$cache_size_kb
AC_SUBST(CACHE_SIZE_KB)

dnl TODO move variables that do not change to another header file
AC_DEFINE(SPLIT_MAX_FACTORS,40,"Maximum number of codelets in a split.")
AC_DEFINE(CODELET_NAME_MAX_SIZE,10,"Maximum size of a codelet name identifier")

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LEX

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
AC_CHECK_HEADERS([stdlib.h,stdio.h,strings.h,math.h,stdbool.h])

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

dnl add gcc warnings, in maintainer mode only
if test "$USE_MAINTAINER_MODE" = yes; then
  CFLAGS="$CFLAGS -Wall -g"
fi

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
