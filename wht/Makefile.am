EXTENSION_FILES = interleave.c vector.c ddl.c concurrent.c external.c 
EXTENSION_DISTS = simd.h

BUILT_SOURCES = scanner.h scanner.c parser.h parser.c 

EXTRA_DIST = wht.h registry.h $(BUILT_SOURCES) $(EXTENSION_DISTS)

AM_YFLAGS = -d
AM_LFLAGS = --header-file=scanner.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/wht/codelets


SUBDIRS = codelets 

libwht_la_SOURCES = node.c direct.c small.c split.c utils.c wht.c rule.c \
										scanner.l parser.y $(EXTENSION_FILES)

lib_LTLIBRARIES = libwht.la

libwht_la_LIBADD	= $(top_builddir)/wht/codelets/libwht_codelets.la


# TODO should utilize WHT_SUFFIX to disambiguate types
bin_PROGRAMS					= wht_verify wht_attach wht_classify

wht_verify_SOURCES		= verify.c 
wht_verify_LDADD			= libwht.la

wht_attach_SOURCES		= attach.c 
wht_attach_LDADD			= libwht.la

wht_classify_SOURCES	= classify.c 
wht_classify_LDADD		= libwht.la

if MAINTAINER_MODE	
# NOTE: This is a hack to fix a problem with autoconf ylwrap script
# See: http://lists.gnu.org/archive/html/automake/2007-11/msg00001.html
scanner.h:
	$(LEX) --outfile=scanner.c scanner.l
endif # MAINTAINER_MODE

