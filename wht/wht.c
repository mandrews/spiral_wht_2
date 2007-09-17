#include "wht.h"
#include "registry.h"

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

void
wht_guard(Wht *W, size_t right)
{
  /* Empty */
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
  W->guard     = wht_guard;
  W->nILNumber = 1;

  return W;  
}

void 
wht_free_codelet(Wht *W) 
{
  wht_free(W);
}

Wht **
wht_leaf_nodes(size_t size)
{
  wht_family *p;
  Wht **W;
  Wht **Ws;
  Wht **nodes;

  size_t i;

  nodes = wht_malloc(sizeof(*nodes) * WHT_MAX_FAMILY);

  for (p = wht_family_generators, i = 0; *p != NULL; ++p) {
    Ws = (*p)(size);

    for (W = Ws; *W != NULL; ++W, ++i) 
      nodes[i] = *W;
  }

  nodes[i] = NULL;

  return nodes;
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
