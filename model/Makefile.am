AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht -I$(top_srcdir)/wht/store -I$(top_srcdir)/measure

bin_PROGRAMS = ic wht_measure_count

ic_SOURCES = ic.cxx labeled_vector.cxx linalg.c linalg.h

ic_LDADD	 = $(top_builddir)/wht/libwht.la \
	$(top_builddir)/measure/libmeasure.la 

wht_measure_count_SOURCES = measure_count.cxx labeled_vector.cxx linalg.c linalg.h

wht_measure_count_LDADD	 = $(top_builddir)/wht/libwht.la \
	$(top_builddir)/measure/libmeasure.la 


