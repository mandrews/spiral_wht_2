/**
 * \file registry.h
 *
 * \brief Mechanism for registering new codelets into the WHT Package.
 *
 * Each new transform registered should implement the \ref codelet_transform_fp
 * interface.  Registration is by:
 *
 * -# Defining the external symbol outside the table
 * \code
 * extern void my_transform(Wht *W);
 * \endcode
 * -# Defining the entry inside the table
 * \code
 * SMALL_TRANSFORM_ENTRY("mysmall",  0, my_transform),
 * \endcode
 *
 * Using macro guards to conditionally include entries
 * is recommended since it
 * will decrease bloat in the table.  Typically, macro guards are naturally determined 
 * from configuration parameters.
 */

#ifndef REGISTRY_H
#define REGISTRY_H

#include "wht.h"

/**
 * \brief Structure for registering new codelet transforms with the package.
 *
 * \param name    Identifier associated with transform
 * \param params  Number of parameters accepted by transform
 * \param call    Interface to transform codelet
 */
typedef struct {
  char    name[MAX_CODELET_NAME_SIZE];
  size_t  params;
  bool    small;
  codelet_transform_fp call;
} codelet_transform_entry;

/**
 * \brief Registers split transform 
 *
 * \param NAME  Identified associated with transform
 * \param N     Number of parameters
 * \param FUNC  Transform function
 */
#define SPLIT_TRANSFORM_ENTRY(NAME, N, FUNC) \
  { NAME, N, false, (codelet_transform_fp) &FUNC }

/**
 * \brief Registers small transform 
 * \param NAME  Identified associated with transform
 * \param N     Number of parameters
 * \param FUNC  Transform function
 */
#define SMALL_TRANSFORM_ENTRY(NAME, N, FUNC) \
  { NAME, N,  true, (codelet_transform_fp) &FUNC }

#define TRANSFORM_ENTRY_END { "", 0, true, NULL } 
  /**< Place this at the end of the transform_registry to halt iteration */


/* Transform extensions to core package */
#if ((WHT_MAX_INTERLEAVE > 0) || (WHT_VECTOR_SIZE > 0))
extern void split_interleave_transform(Wht *W);
#endif

#if ((WHT_MAX_INTERLEAVE > 0)) 
extern void small_interleave_transform(Wht *W);
#endif

#if ((WHT_MAX_INTERLEAVE > 0) && (WHT_VECTOR_SIZE > 0))
extern void small_vector_transform(Wht *W);
#endif

#if ((WHT_VECTOR_SIZE > 0))
extern void small_right_vector_transform(Wht *W);
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
