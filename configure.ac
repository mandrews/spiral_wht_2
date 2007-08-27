dnl Process this file with autoconf to produce a configure script.
AC_INIT(spiral_wht, 2.0, mjand@drexel.edu)

AM_INIT_AUTOMAKE(1.10)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
AC_DISABLE_STATIC
AM_DISABLE_STATIC

AC_CANONICAL_HOST

AC_ARG_ENABLE(
  single, 
  [AC_HELP_STRING([--enable-single], [compile wht in single precision])], 
  ok=$enableval, ok=no)

if test "$ok" = "yes"; then
	AC_DEFINE(WHT_SINGLE,1,[Define to compile in single precision.])
	PRECISION=s
else
	PRECISION=d
fi
AM_CONDITIONAL(SINGLE, test "$ok" = "yes")

AC_SUBST(PRECISION)

dnl compute library suffix
case "$PRECISION" in
     s) PREC_SUFFIX=f;;
     d) PREC_SUFFIX=;;
esac
AC_SUBST(PREC_SUFFIX)

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
