EXTRA_DIST = count_sse.sh

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht 

bin_PROGRAMS = wht_count wht_predict

wht_count_SOURCES = count.h count.cxx count_main.cxx

wht_predict_SOURCES = count.h count.cxx predict_main.cxx

wht_count_LDADD	 = $(top_builddir)/wht/libwht.la 

wht_predict_LDADD	 = $(top_builddir)/wht/libwht.la 
