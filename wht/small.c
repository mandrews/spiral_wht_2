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
 * \file small.c
 * \author Michael Andrews
 *
 * \brief Small unrolled codelet rule.
 *
 */

#include "wht.h"
#include "codelets.h"

Wht *
small_init(size_t n)
{
  Wht *W;

  W            = node_init(n, "small");
  W->apply     = NULL; /* Ensure that null_apply is overridden */
  W->apply     = codelet_apply_lookup(W);

  if (n > WHT_MAX_UNROLL)
    error_msg_set(W, "not configured for unrolled codelets of size %zd", n);

  if (W->apply == NULL)
    error_msg_set(W, "could not find codelet");

  return W;  
}

