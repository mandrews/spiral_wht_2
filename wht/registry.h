#ifndef REGISTRY_H
#define REGISTRY_H

#include "wht.h"

/* Transform extensions to core package */
void split_interleave_transform(Wht *W, int params[], size_t n);
void small_interleave_transform(Wht *W, int params[], size_t n);

static const codelet_transform_entry 
codelet_transforms_registry[] = {
  { "splitil" ,     0, (codelet_transform_fp) &split_interleave_transform},
  { "smallil" ,     1, (codelet_transform_fp) &small_interleave_transform},
  TRANSFORM_ENTRY_END /* This halts iteration on the registry */
};

#endif/*REGISTRY_H*/
