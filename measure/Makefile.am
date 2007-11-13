if PAPI_MEASURE
OPT_SUBDIRS = papi
OPT_CPPLAGS = -I $(PAPI_PATH)/include
else
OPT_SUBDIRS =
OPT_CPPLAGS = 
endif

SUBDIRS = usec $(OPT_SUBDIRS)

AM_CPPFLAGS = -I$(top_srcdir)/wht -I$(top_srcdir)/measure/papi $(OPT_CPPLAGS)

noinst_LTLIBRARIES = libmeasure.la

libmeasure_la_SOURCES = measure.c

libmeasure_la_LIBADD  = $(top_builddir)/wht/libwht.la \
	$(top_builddir)/measure/papi/libpapi_utils.la


