#ifndef REGISTRY_H
#define REGISTRY_H

#include "wht.h"

/* Split Extensions to core package */
Wht * transpose_init(char *name, Wht *Ws[], size_t nn, int params[], size_t np);
Wht * split_interleave_init(char *name, Wht *Ws[], size_t nn, int params[], size_t np);

static const split_init_entry
splits_registry[] = {
  { "split",        0, (split_init_fp) &split_init },
  { "splitddl",     0, (split_init_fp) &transpose_init },
  { "splitil",      0, (split_init_fp) &split_interleave_init },
  SPLIT_INIT_ENTRY_END /* This halts iteration on the registry */
};

/* Small Extensions to core package */
Wht * interleave_init(char *name, size_t n, int params[], size_t np);
Wht * right_vector_init(char *name, size_t n, int params[], size_t np);
Wht * interleave_vector_init(char *name, size_t n, int params[], size_t np);

static const small_init_entry
smalls_registry[] = {
  { "small"   , 0, (small_init_fp) &small_init },
  { "smallil" , 1, (small_init_fp) &interleave_init},
  { "smallv"  , 1, (small_init_fp) &right_vector_init },
  { "smallv"  , 2, (small_init_fp) &interleave_vector_init },
  SMALL_INIT_ENTRY_END /* This halts iteration on the registry */
};


#endif/*REGISTRY_H*/
