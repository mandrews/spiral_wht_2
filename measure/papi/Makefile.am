EXTRA_DIST=papi_utils.h
AM_CPPFLAGS = -I$(top_srcdir)/wht -I$(top_srcdir)/measure -I$(srcdir) -I$(PAPI_PATH)/include \
	-D MAX_CACHE_SIZE_KB=$(CACHE_SIZE_KB)

#if PAPI_MEASURE
#bin_PROGRAMS = wht_measure
#else
#noinst_PROGRAMS = wht_measure
#endif


noinst_LTLIBRARIES = libpapi_utils.la

#wht_measure_SOURCES = measure.c papi_utils.c invnorm.c
#wht_measure_LDADD   = $(top_builddir)/wht/libwht.la $(PAPI_PATH)/lib/libpapi.so
# This hard codes the papi library path into the executable
#wht_measure_LDFLAGS = -rpath $(PAPI_PATH)/lib

libpapi_utils_la_SOURCES = papi_utils.c 
libpapi_utils_la_LIBADD  = $(top_builddir)/wht/libwht.la $(PAPI_PATH)/lib/libpapi.so
# This hard codes the papi library path into the executable
#libpapi_utils_la_LDFLAGS = -rpath $(PAPI_PATH)/lib

