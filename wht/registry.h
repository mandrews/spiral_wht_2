/**
 * \file registry.h
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


/* Transform extensions to core package */

void split_interleave_rule(Wht *W);

void small_interleave_rule(Wht *W);

void small_vector_rule(Wht *W);

void small_right_vector_rule(Wht *W);

static const rule 
rule_registry[] = {

#if ((WHT_MAX_INTERLEAVE > 0) || (WHT_VECTOR_SIZE > 0))
  SPLIT_RULE_ENTRY("splitil", 0, split_interleave_rule),
#endif

#if ((WHT_MAX_INTERLEAVE > 0)) 
  SMALL_RULE_ENTRY("smallil", 1, small_interleave_rule),
#endif

#if ((WHT_MAX_INTERLEAVE > 0) && (WHT_VECTOR_SIZE > 0))
  SMALL_RULE_ENTRY("smallv",  3, small_vector_rule),
#endif

#if ((WHT_VECTOR_SIZE > 0))
  SMALL_RULE_ENTRY("smallv",  1, small_right_vector_rule),
#endif

  RULE_ENTRY_END /* This halts iteration on the registry */
};

#endif/*REGISTRY_H*/
