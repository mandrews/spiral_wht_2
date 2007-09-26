#ifndef REGISTRY_H
#define REGISTRY_H

#include "wht.h"

extern split_init_fp split_init;

static const split_init_entry
splits_registry[] = {
  { "split",    0, (split_init_fp) &split_init },
  SPLIT_INIT_ENTRY_END /* This halts the iteration of the registry */
};

extern small_init_fp small_init;
extern small_init_fp wht_init_interleave;
extern small_init_fp wht_init_interleave_vector;
extern small_init_fp wht_init_right_vector;

static const small_init_entry
smalls_registry[] = {
  { "small"   , 0, (small_init_fp) &small_init },
  { "smallil" , 1, (small_init_fp) &wht_init_interleave },
  { "smallv"  , 1, (small_init_fp) &wht_init_right_vector },
  { "smallv"  , 2, (small_init_fp) &wht_init_interleave_vector },
  SMALL_INIT_ENTRY_END /* This halts the iteration of the registry */
};


#endif/*REGISTRY_H*/
