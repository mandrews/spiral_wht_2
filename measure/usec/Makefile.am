AM_CPPFLAGS = -I$(top_srcdir)/wht 

if PAPI_MEASURE
noinst_PROGRAMS = wht_measure
else
bin_PROGRAMS = wht_measure
endif

wht_measure_SOURCES = measure.c
wht_measure_LDADD   = $(top_builddir)/wht/libwht.la
