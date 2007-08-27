AM_CPPFLAGS = -I$(top_srcdir)/wht -I$(srcdir) -I$(papi_path)/include

noinst_PROGRAMS = measure

measure_SOURCES = measure.c papi_utils.c
measure_LDADD   = $(top_builddir)/wht/libwht.la $(papi_path)/lib/libpapi.so
