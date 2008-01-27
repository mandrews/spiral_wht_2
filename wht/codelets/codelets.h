/*
 * Copyright (c) 2007 Drexel University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/**
 * \file codelets.h
 * \author Michael Andrews
 *
 * \brief Helper functions to load generated codelets.
 */

#ifndef CODELETS_H
#define CODELETS_H

#include "wht.h"

/**
 * \brief Structure for registering new computational (unrolled) codelets with the package.
 *
 * \see codelets_registry.h
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

/**
 * \brief Lookup a codelet based on the Wht::to_string method
 *
 * \return The function pointer to the codelet.
 */
codelet_apply_fp codelet_apply_lookup(Wht *W);

#endif/*CODELETS_H*/

