if PAPI_MEASURE
OPT_SUBDIRS = papi
OPT_CPPLAGS = -I $(PAPI_PATH)/include
else
OPT_SUBDIRS =
OPT_CPPLAGS = 
endif

SUBDIRS = $(OPT_SUBDIRS)

AM_CPPFLAGS = -I$(top_srcdir)/wht -I$(top_srcdir)/measure/papi $(OPT_CPPLAGS)

noinst_LTLIBRARIES = libmeasure.la

libmeasure_la_SOURCES = stat.h stat.c measure.c measure.h invnorm.c

libmeasure_la_LIBADD  = $(top_builddir)/wht/libwht.la \
	$(top_builddir)/measure/papi/libpapi_utils.la


bin_PROGRAMS = wht_measure

wht_measure_SOURCES = main.c

wht_measure_LDADD = libmeasure.la \
	$(PAPI_PATH)/lib/libpapi.so

# This hard codes the papi library path into the executable
wht_measure_LDFLAGS = -rpath $(PAPI_PATH)/lib
