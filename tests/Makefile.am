EXTRA_DIST = correctness.rb omp.pl
TESTS_ENVIRONMENT = BUILDDIR=$(top_builddir) $(RUBY)
TESTS = correctness.rb 

check-local:
	OMP_NUM_THREADS=2 $(PERL) $(top_builddir)/tests/omp.pl -n $(MAX_UNROLL) -k $(MAX_INTERLEAVE)

