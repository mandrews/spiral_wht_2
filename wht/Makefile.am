EXTRA_DIST = wht.h wht_vector.h registry.h
BUILT_SOURCES = scanner.c parser.h parser.c

AM_YFLAGS = -d

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht/codelets

SUBDIRS = codelets 

lib_LTLIBRARIES = libwht.la

libwht_la_SOURCES = direct.c interleave.c vector.c small.c split.c \
									  utils.c wht.c scanner.l parser.y
libwht_la_LIBADD  = $(top_builddir)/wht/codelets/libwht_codelets.la

# TODO should utilize WHT_SUFFIX to disambiguate types
bin_PROGRAMS 				= wht_verify
wht_verify_SOURCES	= verify.c 
wht_verify_LDADD		= libwht.la

