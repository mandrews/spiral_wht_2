#ifndef REGISTRY_H
#define REGISTRY_H

#include "wht.h"

/* Transform extensions to core package */
void split_interleave_transform(Wht *W);
void small_interleave_transform(Wht *W);
void small_vector_transform(Wht *W);
void small_right_vector_transform(Wht *W);

static const codelet_transform_entry 
codelet_transforms_registry[] = {
  /* XXX: TRANSFORM_ENTRY() */
  { "splitil" ,     0, (codelet_transform_fp) &split_interleave_transform},
  { "smallil" ,     1, (codelet_transform_fp) &small_interleave_transform},
  { "smallv" ,      3, (codelet_transform_fp) &small_vector_transform},
  { "smallv" ,      1, (codelet_transform_fp) &small_right_vector_transform},
  TRANSFORM_ENTRY_END /* This halts iteration on the registry */
};

#endif/*REGISTRY_H*/
