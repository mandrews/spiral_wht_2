#ifndef CODELETS_H
#define CODELETS_H

#include "wht.h"

/**
 * \brief Structure for registering new computational (unrolled) codelets with the package.
 *
 * \see codelets/codelets_registry.h
 *
 * \param name    Identifier associated with codelet
 * \param call    Interface to initialize codelet codelet
 */
typedef struct {
  char  *name;
  codelet_apply_fp call;
} codelet_apply_entry;

#define CODELET_APPLY_ENTRY(NAME,FUNC) \
  { NAME, (codelet_apply_fp) &FUNC }

#define CODELET_APPLY_ENTRY_END { "", (codelet_apply_fp) NULL }
  /**< Place this at the end of the transform_registry to halt iteration */

codelet_apply_fp codelet_apply_lookup(Wht *W);

#endif/*CODELETS_H*/

