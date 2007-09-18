#include "wht.h"
#include "registry.h"

/* Defined in parser.c which is generated by parser.y  */
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
wht_init_codelet(int n, char *name)
{
  Wht *W;

  W            = (Wht *) wht_malloc(sizeof(Wht));
  W->N         = (1 << n);
  W->n         = n;
  W->free      = wht_free_codelet;
  W->apply     = NULL;
  W->guard     = wht_guard;
  W->name      = name;

  W->attr[interleave_by] = 1;

  return W;  
}

void 
wht_free_codelet(Wht *W) 
{
  wht_free(W);
}

#if 0
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
#endif

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


split 
lookup_split(const char *name, size_t params)
{
  int i;
  split_entry *p;

  for (p = (split_entry *) splits_registry; p != NULL && (split) p->call != NULL; ++p)
    if ((strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) && (params == p->params))
      return p->call;
  return NULL;
}

small 
lookup_small(const char *name, size_t params)
{
  int i;
  small_entry *p;

  for (p = (small_entry *) smalls_registry; p != NULL && (small) p->call != NULL; ++p)
    if ((strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) && (params == p->params))
      return p->call;
  return NULL;
}

