OPTIONS_AUTOMAKE=gnu

data_DATA =

EXTRA_DIST = doc autogen.sh autodoc.sh extra

# TODO add model when ready to be dist
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
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh 'small[1]' shuffle
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh 'small[1]' unpack
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh 'small[1]' scalar_add
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh 'small[1]' vector_add
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh 'small[1]' scalar_mov
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh 'small[1]' vector_mov
	$(top_srcdir)/install-sh $(top_srcdir)/model/share/* $(datadir)
	$(top_srcdir)/install-sh $(top_srcdir)/model/ic/count_sse.sh $(bindir)
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_dp $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_query $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_search $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_update $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_util $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/utils.rb $(prefix)/extra
	rm -rf $(top_srcdir)/model/share
