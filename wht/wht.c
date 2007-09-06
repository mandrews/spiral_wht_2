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
  fprintf(stderr,"CC: %s\n", WHT_CC);
  fprintf(stderr,"CFLAGS: %s\n", WHT_CFLAGS);
  fprintf(stderr,"type: %s\n", WHT_TYPE_STRING);
  fprintf(stderr,"max_unroll: %d\n", WHT_MAX_UNROLL);
  fprintf(stderr,"vector_size: %d\n", WHT_VECTOR_SIZE);
}
