EXTRA_DIST=papi_utils.h
AM_CPPFLAGS = -I$(top_srcdir)/wht -I$(srcdir) -I$(papi_path)/include \
	-D MAX_CACHE_SIZE_KB=$(CACHE_SIZE_KB)

noinst_PROGRAMS = measure

measure_SOURCES = measure.c papi_utils.c
measure_LDADD   = $(top_builddir)/wht/libwht.la $(papi_path)/lib/libpapi.so
# NOTE once install dir stuff is settled this should not be necessary
measure_LDFLAGS = -rpath $(papi_path)/lib
