BUILT_SOURCES = scanner.c 

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht/codelets

SUBDIRS = codelets 

noinst_PROGRAMS = verify

verify_SOURCES = verify.c direct.c vector.c small.c split.c utils.c wht.c scanner.l
verify_LDADD = $(top_builddir)/wht/codelets/libwht_codelets.la
