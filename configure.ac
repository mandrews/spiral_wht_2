#  Process this file with autoconf to produce a configure script.
AC_INIT([spiral_wht],[2.0-pre-4],[mjand@drexel.edu])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(1.9)
AM_MAINTAINER_MODE
AC_DISABLE_SHARED
AM_DISABLE_SHARED

AC_CANONICAL_HOST

# 
#  M A I N T A I N E R   P A R A M E T E R S
# 
MAX_UNROLL_PKG=8
MAX_INTERLEAVE_PKG=16
MAX_VECTOR_PKG=4

AC_SUBST(MAX_UNROLL_PKG)
AC_SUBST(MAX_INTERLEAVE_PKG)
AC_SUBST(MAX_VECTOR_PKG)



# 
#  D E F A U L T   P A R A M E T E R S
# 
MAX_UNROLL=6
MAX_INTERLEAVE=0
VECTOR_SIZE=0

if test "$CFLAGS" = ""; then
  CFLAGS="-O2"
fi

# 
#  T R A N S F O R M   T Y P E
# 
AC_ARG_ENABLE([double], 
  [AS_HELP_STRING([--enable-double],
    [compile with double precision floating point (default is enabled) ])], 
  [ok=$enableval], 
  [ok=yes])

if test "$ok" = "yes"; then
	AC_DEFINE(WHT_DOUBLE,1,
    [define to compile with double precision floating point])
  WHT_PRECISION=
  WHT_SUFFIX=
fi

AC_ARG_ENABLE([float], 
  [AS_HELP_STRING([--enable-float],
   [compile wht with single precision floating point (default is disabled)])], 
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



# 
#  M A X    C O D E L E T    S I Z E
# 
AC_ARG_ENABLE([max_unroll], 
  [AS_HELP_STRING([--enable-max-unroll=N],
    [maximum unrolled codelet size (default is 6)])],
  [max_unroll=$enableval], 
  [max_unroll=$MAX_UNROLL])

if test $max_unroll -gt $MAX_UNROLL_PKG; then
  AC_MSG_ERROR([--enable-max-unroll larger than package was built for])
fi

AC_DEFINE_UNQUOTED(WHT_MAX_UNROLL,$max_unroll,
  [maximum unrolled codelet size])



# 
#  M A X   I N T E R L E A V E    F A C T O R 
# 
AC_ARG_ENABLE([interleave], 
  [AS_HELP_STRING([--enable-interleave=K],
    [interleave K loop iterations in codelet (default is disabled)])],
  [interleave=$enableval], 
  [interleave=$MAX_INTERLEAVE])

if test $interleave -gt $MAX_INTERLEAVE_PKG; then
  AC_MSG_ERROR([--enable-interleave larger than package was built for])
fi


# 
#  V E C T O R I Z A T I O N
# 
AC_ARG_ENABLE([sse], 
  [AS_HELP_STRING([--enable-sse],
    [enable SSE optimizations (default is disabled)])], 
  [have_sse=$enableval], 
  [have_sse=no])

if test "$have_sse" = "yes"; then
	AC_DEFINE(WHT_HAVE_SSE,1,
    [define to enable SSE optimizations.])

	have_simd="yes"
	if test "$WHT_PRECISION" != "f"; then
		AC_MSG_ERROR([--enable-sse can only be used in conjunction with --enable-float])
	fi

  CFLAGS="$CFLAGS -msse"

  VECTOR_SIZE=4

  if test $VECTOR_SIZE -gt $MAX_VECTOR_PKG; then
    AC_MSG_ERROR([--enable-sse vectors are larger than package was built for])
  fi

  if test $interleave -eq 0; then
    interleave=$VECTOR_SIZE;
  fi

fi

AC_ARG_ENABLE([sse2], 
  [AS_HELP_STRING([--enable-sse2],
    [enable SSE2 optimizations (default is disabled)])], 
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

  CFLAGS="$CFLAGS -msse2"

  VECTOR_SIZE=2

  if test $VECTOR_SIZE -gt $MAX_VECTOR_PKG; then
    AC_MSG_ERROR([--enable-sse2 vectors are larger than package was built for])
  fi

  if test $interleave -eq 0; then
    interleave=$VECTOR_SIZE;
  fi

fi

AC_DEFINE_UNQUOTED(WHT_VECTOR_SIZE,$VECTOR_SIZE, [Size of vector to use.])

AC_DEFINE_UNQUOTED(WHT_MAX_INTERLEAVE,$interleave,
  [interleave K loop iterations in codelet])



# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup getline fgetln])
AC_SEARCH_LIBS([sqrtl],[m],[], AC_MSG_ERROR([required sqrtl symbol not found]))
AC_SEARCH_LIBS([ceil],[m],[], AC_MSG_ERROR([required ceil symbol not found]))


# 
#  P A P I    M E A S U R E
# 
AC_ARG_WITH([papi], 
  [AS_HELP_STRING([--with-papi=PATH],
    [compile with PAPI support, PATH is location of PAPI install])], 
  [papi_path=$withval], 
  [papi_path="no"])

AM_CONDITIONAL(HAVE_PAPI, test "$papi_path" != "no")

if test "$papi_path" != "no"; then
  LDFLAGS="$LDFLAGS -L$papi_path/lib"
  CPPFLAGS="$CPPFLAGS -I$papi_path/include"

  AC_DEFINE(HAVE_PAPI, [], [Compile with PAPI] )

  AC_CHECK_HEADER([papi.h],[],
    AC_MSG_ERROR([required header file for --with-papi not found]))

  LIBS="$LIBS -lpapi"
fi

#  Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CHECK_PROG(PERL,perl,perl)
AC_CHECK_PROG(RUBY,ruby,ruby)
AC_CHECK_PROG(BASH,bash,bash)
AC_PROG_LIBTOOL

if test "$USE_MAINTAINER_MODE" = yes; then
AC_PROG_LEX
AC_PROG_YACC
fi

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_CHECK_HEADERS([inttypes.h libintl.h malloc.h math.h stddef.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_C_VOLATILE

#  Add gcc warnings, in maintainer mode only
if test "$USE_MAINTAINER_MODE" = yes; then
  CFLAGS="$CFLAGS -Wall -g"
fi

if test "$ac_test_CXXFLAGS" != "set"; then 
  CXXFLAGS="$CFLAGS"
fi

AC_DEFINE_UNQUOTED(WHT_CC,"$CC",
  [the compiler used to compile this.])
AC_DEFINE_UNQUOTED(WHT_CFLAGS,"$CFLAGS",
  [the compiler flags used to compile this.])

# TODO puts these back when model is ready to be distributed
# model/Makefile
# model/ic/Makefile
AC_CONFIG_FILES([
   Makefile
   whtgen/Makefile
   wht/Makefile
   wht/codelets/Makefile
   measure/Makefile
   measure/papi/Makefile
   search/Makefile
   rand/Makefile
   tests/Makefile
])

AC_OUTPUT
