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
	mkdir -p $(prefix)/share
	mkdir -p $(top_srcdir)/model/share
	mkdir -p $(prefix)/extra
	mkdir -p $(prefix)/scripts
	$(top_srcdir)/install-sh $(top_srcdir)/model/share/* $(datadir)
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_dp $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_query $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_update $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_wrap $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_strip $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_interleave $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_vectorize $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_vectorize_2 $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/utils.rb $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/scripts/* $(prefix)/scripts
	rm -rf $(top_srcdir)/model/share
