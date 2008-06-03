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
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'unpckhpd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'unpcklpd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addsd|subsd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addsd|subsd' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addpd|subpd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addpd|subpd' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movsd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movsd' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movapd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movapd' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movhpd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movhpd' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movlpd'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movlpd' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addss|subss'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addss|subss' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addps|subps'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'addps|subps' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movss'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movss' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movaps'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movaps' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movhps'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movhps' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movlps'
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'movlps' -m
	$(BASH) $(top_srcdir)/model/ic/count_sse.sh -w 'small[1]' -i 'shufps'
	$(top_srcdir)/install-sh $(top_srcdir)/model/share/* $(datadir)
	$(top_srcdir)/install-sh $(top_srcdir)/model/ic/count_sse.sh $(bindir)
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_dp $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_query $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_update $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_wrap $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_sample $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_strip $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_interleave $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/wht_vectorize $(prefix)/extra
	$(top_srcdir)/install-sh $(top_srcdir)/extra/utils.rb $(prefix)/extra
	rm -rf $(top_srcdir)/model/share
