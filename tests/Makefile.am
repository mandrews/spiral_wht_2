EXTRA_DIST = correctness.rb omp.pl
TESTS_ENVIRONMENT = BUILDDIR=$(top_builddir) \
	MAX_UNROLL=$(MAX_UNROLL) \
	MAX_INTERLEAVE=$(MAX_INTERLEAVE) \
	VECTOR_SIZE=$(VECTOR_SIZE) \
	$(PERL) 

TESTS = default.pl 

if HAVE_INTERLEAVE
TESTS += interleave.pl
endif # MAX_INTERLEAVE

if HAVE_VECTOR
TESTS += vector.pl
endif # MAX_VECTOR

if HAVE_OMP
TESTS += omp.pl
endif # HAVE_OMP

