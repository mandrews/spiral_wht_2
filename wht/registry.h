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
 * \file registry.h
 * \author Michael Andrews
 *
 * \brief Mechanism for registering new codelets into the WHT Package.
 *
 * Each new rule registered should implement the \ref rule_fp
 * interface.  Registration is by:
 *
 * -# Defining the symbol outside the table
 * \code
 * void my_rule(Wht *W);
 * \endcode
 * -# Defining the entry inside the table
 * \code
 * SMALL_RULE_ENTRY("mysmall",  0, my_rule),
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
 * \brief Registers split rule
 *
 * \param NAME  Identifier associated with rule
 * \param N     Number of parameters associated with rule
 * \param FUNC  Rule attachment function
 */
#define SPLIT_RULE_ENTRY(NAME, N, FUNC) \
  { NAME, N, {}, false, (rule_fp) &FUNC }

/**
 * \brief Registers small rule
 *
 * \param NAME  Identifier associated with rule
 * \param N     Number of parameters associated with rule
 * \param FUNC  Rule attachment function
 */
#define SMALL_RULE_ENTRY(NAME, N, FUNC) \
  { NAME, N,  {}, true, (rule_fp) &FUNC }

#define RULE_ENTRY_END { "", 0, {}, true, NULL } 
  /**< Place this at the end of the rule_registry to halt iteration */



/** 
 * \brief Possible attributes associated with a node after a rule has been applied
 */
enum attribute_names
{ 
  INTERLEAVE_BY = 0,
	INTERLEAVING,
  VECTOR_SIZE, 
  VECTOR_STRIDE, 
  VECTOR_SQUARE_STRIDE, /* Stride N / v^2 */
	BLOCK_SIZE
};



/* Transform extensions to core package */

void split_vector_rule(Wht *W);

void split_interleave_rule(Wht *W);

void split_parallel_rule(Wht *W);

void split_ddl_parallel_rule(Wht *W);

void split_ddl_rule(Wht *W);



void small_interleave_rule(Wht *W);

void small_vector_rule_1(Wht *W);
void small_vector_rule_2(Wht *W);
void small_vector_rule_3(Wht *W);

void small_external_rule(Wht *W);

static const rule 
rule_registry[] = {

#if ((WHT_MAX_INTERLEAVE > 0) || (WHT_VECTOR_SIZE > 0))
  SPLIT_RULE_ENTRY("splitil", 0, split_interleave_rule),
#endif

#if ((WHT_HAVE_OMP))
  SPLIT_RULE_ENTRY("splitp",  0, split_parallel_rule),
#endif

#if ((WHT_HAVE_OMP))
  SPLIT_RULE_ENTRY("splitddlp",  1, split_ddl_parallel_rule),
#endif

#ifdef WHT_WITH_DDL
  SPLIT_RULE_ENTRY("splitddl", 1, split_ddl_rule),
#endif

#if ((WHT_MAX_INTERLEAVE > 0)) 
  SMALL_RULE_ENTRY("smallil", 1, small_interleave_rule),
#endif

#if ((WHT_VECTOR_SIZE > 0))
  SPLIT_RULE_ENTRY("splitv",  1, split_vector_rule),
  SMALL_RULE_ENTRY("smallv",  3, small_vector_rule_1),
  SMALL_RULE_ENTRY("smallv",  2, small_vector_rule_2),
  SMALL_RULE_ENTRY("smallv",  1, small_vector_rule_3),
#endif

#ifdef HAVE_SPIRAL
  SMALL_RULE_ENTRY("spiral",  0, small_external_rule),
#endif

  RULE_ENTRY_END /* This halts iteration on the registry */
};

#endif/*REGISTRY_H*/
