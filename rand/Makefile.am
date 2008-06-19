AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht
#AM_CXXFLAGS =

bin_PROGRAMS = wht_rand wht_rotate

wht_rand_SOURCES = combin.h rand.cxx combin.cxx
wht_rand_LDADD	 = $(top_builddir)/wht/libwht.la

wht_rotate_SOURCES = combin.h rotate.cxx combin.cxx
wht_rotate_LDADD	 = $(top_builddir)/wht/libwht.la
