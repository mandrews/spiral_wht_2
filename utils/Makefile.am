EXTRA_DIST=rand.h randtree.h tree.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht

bin_PROGRAMS = wht_randtree
wht_randtree_SOURCES = genrandtree.c rand.c randtree.c tree.c
wht_randtree_LDADD	= $(top_builddir)/wht/libwht.la

