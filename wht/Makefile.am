EXTRA_DIST = wht.h wht_vector.h
BUILT_SOURCES = scanner.c parser.h parser.c

AM_YFLAGS = -d

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht/codelets

SUBDIRS = codelets 

noinst_LTLIBRARIES = libwht.la

libwht_la_SOURCES = direct.c interleave.c right_vector.c small.c split.c \
									  utils.c wht.c scanner.l parser.y
libwht_la_LIBADD  = $(top_builddir)/wht/codelets/libwht_codelets.la

noinst_PROGRAMS = verify
verify_SOURCES  = verify.c 
verify_LDADD		= libwht.la

