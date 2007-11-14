AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht -I$(top_srcdir)/measure

bin_PROGRAMS = ic
ic_SOURCES = ic.cxx linalg.c linalg.h

ic_LDADD	 = $(top_builddir)/wht/libwht.la $(top_builddir)/measure/libmeasure.la \
$(PAPI_PATH)/lib/libpapi.so

ic_LDFLAGS = -rpath $(PAPI_PATH)/lib
