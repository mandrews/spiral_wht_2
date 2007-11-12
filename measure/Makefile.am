if PAPI_MEASURE
OPT_SUBDIRS = papi
else
OPT_SUBDIRS =
endif

SUBDIRS = usec $(OPT_SUBDIRS)

AM_CPPFLAGS = -I$(top_srcdir)/wht 

lib_LTLIBRARIES = libwht_measure.la

libwht_measure_la_SOURCES = measure.c

libwht_measure_la_LIBADD  = $(top_builddir)/wht/libwht.la


