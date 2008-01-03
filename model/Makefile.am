AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht -I$(top_srcdir)/wht/store -I$(top_srcdir)/measure

bin_PROGRAMS = wht_measure_count wht_interp_coeff

wht_measure_count_SOURCES = measure_count.cxx labeled_vector.cxx linalg.c linalg.h

wht_measure_count_LDADD	 = $(top_builddir)/wht/libwht.la \
	$(top_builddir)/measure/libmeasure.la 


wht_interp_coeff_SOURCES = interp_coeff.cxx labeled_vector.cxx ic_count.cxx linalg.c linalg.h

wht_interp_coeff_LDADD	 = $(top_builddir)/wht/libwht.la 


