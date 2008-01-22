OPTIONS_AUTOMAKE=gnu

data_DATA =

EXTRA_DIST = doc/html autogen.sh autodoc.sh

# TODO add model when ready to be dist
SUBDIRS = whtgen wht measure search rand tests

# Remove dependancy information
maintainer-clean-local:
	echo 'CODELETS =  ' > $(top_srcdir)/wht/codelets/.codelets

# Automake might be including this because of code generation
dist-hook:
	rm $(top_distdir)/whtgen/whtgen

#installdirs-local:
#	mkdir $(datadir)/share
