#  Process this file with autoconf to produce a configure script.
AC_INIT([spiral_wht],[2.0-pre-5],[mjand@drexel.edu])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(1.9)
AM_MAINTAINER_MODE
AC_DISABLE_SHARED
AM_DISABLE_SHARED

AC_CANONICAL_HOST
AX_COMPILER_VENDOR

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
#  D E F A U L T   C O M P I L E R   F L A G S
# 

case "${ax_cv_c_compiler_vendor}" in
  intel)
    CFLAGS="$CFLAGS -no-gcc -std=c99"
    ;;
esac

# 
#  T R A N S F O R M   T Y P E
# 
AC_ARG_ENABLE([float], 
  [AS_HELP_STRING([--enable-float],
   [compile wht with single precision floating point (default is disabled)])], 
  [ok=$enableval], 
  [ok=no])

if test "$ok" = "yes"; then
	AC_DEFINE(WHT_FLOAT,1,
    [define to compile with single precision floating point])
	AC_DEFINE(WHT_DOUBLE,0,
    [define to compile with double precision floating point])
  WHT_PRECISION=f
  WHT_SUFFIX=f
else
	AC_DEFINE(WHT_DOUBLE,1,
    [define to compile with double precision floating point])
	AC_DEFINE(WHT_FLOAT,0,
    [define to compile with single precision floating point])
  WHT_PRECISION=
  WHT_SUFFIX=
fi



# 
#  M A X    C O D E L E T    S I Z E
# 
AC_ARG_ENABLE([max_unroll], 
  [AS_HELP_STRING([--enable-max-unroll=N],
    [maximum unrolled codelet size (default is 6)])],
  [max_unroll=$enableval], 
  [max_unroll=$MAX_UNROLL])

MAX_UNROLL=$max_unroll

if test $MAX_UNROLL -gt $MAX_UNROLL_PKG; then
  AC_MSG_ERROR([--enable-max-unroll larger than package was built for])
fi

AC_DEFINE_UNQUOTED(WHT_MAX_UNROLL,$MAX_UNROLL,
  [maximum unrolled codelet size])

AC_SUBST(MAX_UNROLL)



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

MAX_INTERLEAVE=$interleave



# 
#  O P E N   M P 
#
AC_ARG_ENABLE([omp], 
  [AS_HELP_STRING([--enable-omp],
    [openMP optimizations (default is disabled)])],
  [have_omp=$enableval], 
  [have_omp=no])

if test "$have_omp" = "yes"; then
	AC_DEFINE(WHT_HAVE_OMP,1,
    [define to enable openMP optimizations.])

  case "${ax_cv_c_compiler_vendor}" in
    gnu)
      CFLAGS="$CFLAGS -fopenmp"
      LIBS="$LIBS -lgomp"
      ;;
    intel)
      CFLAGS="$CFLAGS -openmp"
      LIBS="$LIBS -liomp5 -lpthread"
      ;;
  esac
fi

AM_CONDITIONAL(HAVE_OMP, test "$have_omp" != "no")


# 
#  D D L
# 
AC_ARG_ENABLE([ddl], 
  [AS_HELP_STRING([--enable-ddl],
    [DDL optimizations (default is disabled)])],
  [have_ddl=$enableval], 
  [have_ddl=no])

if test "$have_ddl" = "yes"; then
	AC_DEFINE(WHT_WITH_DDL,1,
    [define to enable DDL optimizations.])
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

  case "${ax_cv_c_compiler_vendor}" in
    gnu)
      CFLAGS="$CFLAGS -msse"
    ;;
  esac

  VECTOR_SIZE=4

  if test $VECTOR_SIZE -gt $MAX_VECTOR_PKG; then
    AC_MSG_ERROR([--enable-sse vectors are larger than package was built for])
  fi

  if test $MAX_INTERLEAVE -eq 0; then
    MAX_INTERLEAVE=$VECTOR_SIZE;
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

  case "${ax_cv_c_compiler_vendor}" in
    gnu)
      CFLAGS="$CFLAGS -msse2"
    ;;
  esac

  VECTOR_SIZE=2

  if test $VECTOR_SIZE -gt $MAX_VECTOR_PKG; then
    AC_MSG_ERROR([--enable-sse2 vectors are larger than package was built for])
  fi

  if test $MAX_INTERLEAVE -eq 0; then
    MAX_INTERLEAVE=$VECTOR_SIZE;
  fi

fi

AC_DEFINE_UNQUOTED(WHT_VECTOR_SIZE,$VECTOR_SIZE, [Size of vector to use.])

AC_SUBST(VECTOR_SIZE)

AM_CONDITIONAL(HAVE_VECTOR, test "$VECTOR_SIZE" != 0)

# Need to export interleaving related variables here
# since they are intertwined with vector size

AC_DEFINE_UNQUOTED(WHT_MAX_INTERLEAVE,$MAX_INTERLEAVE,
  [interleave K loop iterations in codelet])

AC_SUBST(MAX_INTERLEAVE)

AM_CONDITIONAL(HAVE_INTERLEAVE, test "$MAX_INTERLEAVE" != 0)





#
#  S P I R A L   C O D E L E T S 
#
AC_ARG_WITH([spiral], 
  [AS_HELP_STRING([--with-spiral=PATH],
    [compile with SPIRAL unrolled codelets, PATH is location SPIRAL codelets])], 
  [spiral_path=$withval], 
  [spiral_path="no"])

AM_CONDITIONAL(HAVE_SPIRAL, test "$spiral_path" != "no")

if test "$spiral_path" != "no"; then
  AC_DEFINE(HAVE_SPIRAL, [], [compile with SPIRAL codelets] )

  if test ! -e "$spiral_path"; then
    AC_MSG_ERROR([--spiral-path does not exist])
  fi

  case "$spiral_path" in
  /*) abs=1;;
  *) abs=0;;
  esac

  if test $abs = 0; then
    AC_MSG_ERROR([--spiral-path needs to be absolute])
  fi
fi

SPIRAL_PATH=$spiral_path

AC_ARG_ENABLE([spiral_unroll], 
  [AS_HELP_STRING([--enable-spiral-unroll=N],
    [maximum SPIRAL unrolled codelet size (default is 6)])],
  [spiral_unroll=$enableval], 
  [spiral_unroll=$MAX_UNROLL])

SPIRAL_UNROLL=$spiral_unroll

AC_SUBST(SPIRAL_PATH)
AC_SUBST(SPIRAL_UNROLL)


# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strdup])
AC_SEARCH_LIBS([sqrtl],[m],[], AC_MSG_ERROR([required sqrtl symbol not found]))
AC_SEARCH_LIBS([ceil],[m],[], AC_MSG_ERROR([required ceil symbol not found]))

# Internal measurement routines
AC_CHECK_FUNCS([clock_gettime], 
  AC_DEFINE(HAVE_NSEC, [], [Compile with nsec]), [])

AC_CHECK_FUNCS([getrusage], 
  AC_DEFINE(HAVE_USEC, [], [Compile with usec]), [])

AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([[unsigned int lo, hi;]],
    [[__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));]])],
  [AC_DEFINE(HAVE_RDTSC, [], [Compile with rdtsc instruction])], [])


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


#
# M E M O R Y   T R A C E
#
AC_ARG_ENABLE([trace], 
  [AS_HELP_STRING([--enable-trace],
   [compile wht with single precision floating point (default is disabled)])], 
  [ok=$enableval], 
  [ok=no])

if test "$ok" = "yes"; then
	AC_DEFINE(TRACE,1,
    [define to compile with a memory trace to stderr])
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

if test "$LEX" != "flex"; then
  AC_MSG_ERROR([flex required in --enable-maintainer-mode])
fi

AC_PROG_YACC

if test "$YACC" != "bison -y"; then
  AC_MSG_ERROR([bison required in --enable-maintainer-mode])
fi

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
   model/Makefile
   model/ic/Makefile
   model/cm/Makefile
])

AC_OUTPUT
