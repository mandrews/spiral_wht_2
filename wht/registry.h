#ifndef REGISTRY_H
#define REGISTRY_H

#include "wht.h"

#if 0
typedef Wht ** (*wht_family)(size_t n);

extern wht_family small_family;
extern wht_family interleave_family;

wht_family 
wht_family_generators[] = {
  (wht_family) &small_family,
  (wht_family) &interleave_family,
  NULL,
};
#endif

extern split wht_init_split;

static const split_entry
splits_registry[] = {
  { "split",    0, (split) &wht_init_split },
  { "", 0, NULL } /* This halts the iteration of the registry */
};

extern small wht_init_small;
extern small wht_init_interleave;
extern small wht_init_vector;
extern small wht_init_right_vector;

static const small_entry
smalls_registry[] = {
  { "small"   , 0, (small) &wht_init_small },
  { "smallil" , 1, (small) &wht_init_interleave },
  { "", 0, NULL } /* This halts the iteration of the registry */
};


#endif/*REGISTRY_H*/
