EXTRA_DIST=papi_utils.h

AM_CPPFLAGS = -I$(top_srcdir)/wht -I$(top_srcdir)/measure -I$(srcdir) -I$(PAPI_PATH)/include 

noinst_LTLIBRARIES = libpapi_utils.la

libpapi_utils_la_SOURCES = papi_utils.c 
libpapi_utils_la_LIBADD  = $(top_builddir)/wht/libwht.la $(PAPI_PATH)/lib/libpapi.so
