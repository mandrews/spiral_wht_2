#ifndef REGISTRY_H
#define REGISTRY_H

#include "wht.h"

/* Transform extensions to core package */
#if ((WHT_MAX_INTERLEAVE > 0) || (WHT_VECTOR_SIZE > 0))
void split_interleave_transform(Wht *W);
#endif

#if ((WHT_MAX_INTERLEAVE > 0)) 
void small_interleave_transform(Wht *W);
#endif

#if ((WHT_MAX_INTERLEAVE > 0) && (WHT_VECTOR_SIZE > 0))
void small_vector_transform(Wht *W);
#endif

#if ((WHT_VECTOR_SIZE > 0))
void small_right_vector_transform(Wht *W);
#endif

static const codelet_transform_entry 
codelet_transforms_registry[] = {

#if ((WHT_MAX_INTERLEAVE > 0) || (WHT_VECTOR_SIZE > 0))
  SPLIT_TRANSFORM_ENTRY("splitil", 0, split_interleave_transform),
#endif

#if ((WHT_MAX_INTERLEAVE > 0)) 
  SMALL_TRANSFORM_ENTRY("smallil", 1, small_interleave_transform),
#endif

#if ((WHT_MAX_INTERLEAVE > 0) && (WHT_VECTOR_SIZE > 0))
  SMALL_TRANSFORM_ENTRY("smallv",  3, small_vector_transform),
#endif

#if ((WHT_VECTOR_SIZE > 0))
  SMALL_TRANSFORM_ENTRY("smallv",  1, small_right_vector_transform),
#endif

  TRANSFORM_ENTRY_END /* This halts iteration on the registry */
};

#endif/*REGISTRY_H*/
