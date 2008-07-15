OPTIONS_AUTOMAKE=gnu

data_DATA =

EXTRA_DIST = doc autogen.sh autodoc.sh extra

SUBDIRS = whtgen wht measure search rand model tests

# Remove dependancy information
maintainer-clean-local:
	echo 'CODELETS =  ' > $(top_srcdir)/wht/codelets/.codelets

# Automake might be including this because of code generation
dist-hook:
	rm $(top_distdir)/whtgen/whtgen

install-exec-local:
	mkdir -p $(prefix)/extra
	mkdir -p $(prefix)/scripts
	mkdir -p $(prefix)/data
	$(top_srcdir)/install-sh $(top_srcdir)/extra/* $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/scripts/* $(prefix)/scripts

