AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht
noinst_LTLIBRARIES = libwht_codelets.la

WHTGEN = ${top_srcdir}/whtgen/whtgen
S1 = s_1.c
ALL_CODELETS = $(S1) 

BUILT_SOURCES= $(ALL_CODELETS) 

libwht_codelets_la_SOURCES = codelets.c codelets.h $(BUILT_SOURCES)

if MAINTAINER_MODE
s_1.c:  $(WHTGEN)
	$(WHTGEN) -n 1 > $@
s_2.c:  $(WHTGEN)
	$(WHTGEN) -n 2 > $@
endif # MAINTAINER_MODE
