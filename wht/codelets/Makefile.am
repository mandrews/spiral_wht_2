AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht
noinst_LTLIBRARIES = libwht_codelets.la


include .codelets # defines $(CODELETS)

BUILT_SOURCES= codelet_registry.h $(CODELETS) 

libwht_codelets_la_SOURCES = codelets.c codelets.h $(BUILT_SOURCES)

if MAINTAINER_MODE	
codelet_registry.h:
	$(PERL) $(srcdir)/make_codelets.pl -m $(MAX_UNROLL_PKG)
endif # MAINTAINER_MODE

