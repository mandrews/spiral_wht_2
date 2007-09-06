#include "wht.h"

extern Wht *wht_root;

Wht *
wht_parse(char *in)
{
  Wht *wht;

  yy_scan_string(in);
  yyparse();
  wht = wht_root;

  return wht;
}

Wht *
wht_init_codelet(int n)
{
  Wht *W;

  W            = (Wht *) wht_malloc(sizeof(Wht));
  W->N         = (long) pow((double) 2, (double) n);
  W->n         = n;
  W->free      = wht_free_codelet;
  W->apply     = NULL;
  W->nILNumber = 1;

  return W;  
}

void 
wht_free_codelet(Wht *W) 
{
  wht_free(W);
}

void
wht_info(void)
{
  printf("CC: %s\n", WHT_CC);
  printf("CFLAGS: %s\n", WHT_CFLAGS);
  printf("type: %s\n", WHT_TYPE_STRING);
  printf("max_unroll: %d\n", WHT_MAX_UNROLL);
  printf("max_interleave: %d\n", WHT_MAX_INTERLEAVE);
  printf("vector_size: %d\n", WHT_VECTOR_SIZE);
}
