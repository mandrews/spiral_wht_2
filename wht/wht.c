#include "wht.h"
#include "registry.h"

/* This symbol points to the root of the parsed wht tree */
extern Wht *wht_root;

Wht *
parse(char *in)
{
  Wht *wht;

  yy_scan_string(in);
  yyparse();

  wht = wht_root;

  return wht;
}

void
info(void)
{
  printf("CC: %s\n", WHT_CC);
  printf("CFLAGS: %s\n", WHT_CFLAGS);
  printf("type: %s\n", WHT_TYPE_STRING);
  printf("max_unroll: %d\n", WHT_MAX_UNROLL);
  printf("max_interleave: %d\n", WHT_MAX_INTERLEAVE);
  printf("vector_size: %d\n", WHT_VECTOR_SIZE);
}

split_init_fp
split_lookup(const char *name, size_t params)
{
  split_init_entry *p;

  for (p = (split_init_entry *) splits_registry; p != NULL && (split_init_fp) p->call != NULL; ++p)
    if ((strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) && (params == p->params))
      return p->call;
  return NULL;
}

small_init_fp
small_lookup(const char *name, size_t params)
{
  small_init_entry *p;

  for (p = (small_init_entry *) smalls_registry; p != NULL && (small_init_fp) p->call != NULL; ++p)
    if ((strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) && (params == p->params))
      return p->call;
  return NULL;
}

