EXTRA_DIST = wht.h wht_vector.h registry.h
BUILT_SOURCES = scanner.h scanner.c parser.h parser.c 

AM_YFLAGS = -d
AM_LFLAGS = --header-file=scanner.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht/codelets


SUBDIRS = codelets store

lib_LTLIBRARIES = libwht.la

libwht_la_SOURCES = null.c direct.c small.c split.c utils.c wht.c \
										scanner.l parser.y \
										interleave.c vector.c

libwht_la_LIBADD  = $(top_builddir)/wht/codelets/libwht_codelets.la

# TODO should utilize WHT_SUFFIX to disambiguate types
bin_PROGRAMS 					= wht_verify wht_convert wht_classify

wht_verify_SOURCES		= verify.c 
wht_verify_LDADD			= libwht.la

wht_convert_SOURCES		= convert.c 
wht_convert_LDADD			= libwht.la

wht_classify_SOURCES	= classify.c 
wht_classify_LDADD		= libwht.la

# NOTE: This is a hack to fix a problem with autoconf ylwrap script
# See: http://lists.gnu.org/archive/html/automake/2007-11/msg00001.html
scanner.h:
	$(LEX) --outfile=scanner.c scanner.l
