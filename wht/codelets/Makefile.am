AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht
noinst_LTLIBRARIES = libwht_codelets.la

WHTGEN = ${top_srcdir}/whtgen/whtgen
S1 = s_1.c
ALL_CODELETS = $(S1) 

BUILT_SOURCES= $(ALL_CODELETS) 

libwht_codelets_la_SOURCES = codelets.c codelets.h $(BUILT_SOURCES)

if MAINTAINER_MODE
s_%.c:  $(WHTGEN)
	$(WHTGEN) -n $* > $@
endif # MAINTAINER_MODE
