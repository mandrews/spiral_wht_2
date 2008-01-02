BUILT_SOURCES = scanner.h scanner.c parser.h parser.c 

AM_YFLAGS = -d
AM_LFLAGS = --header-file=scanner.h

AM_CPPFLAGS = -I$(top_srcdir) 

lib_LTLIBRARIES = libstore.la

libstore_la_SOURCES = store.c scanner.l parser.y 

# NOTE: This is a hack to fix a problem with autoconf ylwrap script
# See: http://lists.gnu.org/archive/html/automake/2007-11/msg00001.html
scanner.h:
	$(LEX) --outfile=scanner.c scanner.l
