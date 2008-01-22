if HAVE_PAPI
OPT_SUBDIRS = papi
OPT_LIBADDS = $(top_builddir)/measure/papi/libpapi_utils.la
else
OPT_SUBDIRS =
OPT_LIBADDS  = 
endif

SUBDIRS = $(OPT_SUBDIRS)

AM_CPPFLAGS = -I$(top_srcdir)/wht 

noinst_LTLIBRARIES = libmeasure.la

libmeasure_la_SOURCES = stat.h stat.c measure.c measure.h invnorm.c extensions.h

libmeasure_la_LIBADD  = $(top_builddir)/wht/libwht.la $(OPT_LIBADDS)

bin_PROGRAMS = wht_measure

wht_measure_SOURCES = main.c

wht_measure_LDADD = libmeasure.la 

