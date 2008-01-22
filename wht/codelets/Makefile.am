EXTRA_DIST = register_codelets.pl make_interleave_codelets.sh make_simd_codelets.sh make_small_codelets.sh

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht
noinst_LTLIBRARIES = libwht_codelets.la

include .codelets # defines $(CODELETS)

BUILT_SOURCES= codelet_registry.h $(CODELETS) $(top_builddir)/whtgen/whtgen

libwht_codelets_la_SOURCES = codelets.c codelets.h $(BUILT_SOURCES)

if MAINTAINER_MODE	
codelet_registry.h: 
	$(BASH) make_small_codelets.sh -n $(MAX_UNROLL_PKG)
	$(BASH) make_interleave_codelets.sh -n $(MAX_UNROLL_PKG) -k $(MAX_INTERLEAVE_PKG)
	$(BASH) make_simd_codelets.sh -n $(MAX_UNROLL_PKG) -k $(MAX_INTERLEAVE_PKG) -v $(MAX_VECTOR_PKG)
	$(PERL) register_codelets.pl $(srcdir)
endif # MAINTAINER_MODE

