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
 * \file codelets.c
 * \author Michael Andrews
 *
 * \brief Helper functions to load generated codelets.
 */

#include "codelets.h"
#include "codelet_registry.h"

codelet_apply_fp 
codelet_apply_lookup(Wht *W)
{
  codelet_apply_entry *p;
  codelet_apply_fp call;
  char *name;

  name = name_to_string(W);

  call  = NULL; /* Did not find codelet in table */

  p = (codelet_apply_entry *) codelet_apply_registry; 
  for (; p != NULL && (codelet_apply_fp) p->call != NULL; ++p)
    if (strcmp(name, p->name) == 0) {
      call = p->call;
      break;
    }

  i_free(name);

  return call;
}

