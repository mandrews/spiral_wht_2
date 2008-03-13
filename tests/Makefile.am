EXTRA_DIST = correctness.rb omp.pl
TESTS_ENVIRONMENT = BUILDDIR=$(top_builddir) $(RUBY)
TESTS = correctness.rb 

check-local:
	$(PERL) $(top_builddir)/tests/omp.pl -n $(MAX_UNROLL) -k $(MAX_INTERLEAVE)

