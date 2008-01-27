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
 * \file rule.c
 * \author Michael Andrews
 *
 * \brief Implementation of %rule functions.
 *
 */

#include "wht.h"
#include "registry.h"
#include "codelets.h"

rule *
rule_init(char *name)
{
  size_t i;
  rule *rule;

  rule = i_malloc(sizeof(*rule));

  rule->n         = 0;
  rule->is_small  = true;
  rule->call      = NULL;

  for (i = 0; i < MAX_RULE_PARAMS; i++)
    rule->params[i] = UNSET_PARAMETER;

  strncpy(rule->name, name, MAX_RULE_NAME_SIZE);

  return rule;
}

rule *
rule_copy(rule *src)
{
  int i;
  rule *dst;

  dst = rule_init(src->name);

  dst->n        = src->n;
  dst->is_small = src->is_small;
  dst->call     = src->call;

  for (i = 0; i < MAX_RULE_PARAMS; i++)
    dst->params[i] = src->params[i];

  return dst;
}

void
rule_free(rule *rule)
{
  i_free(rule);
}

void
rule_eval(Wht *W)
{
  size_t i, nn;

  if (W->rule->call != NULL)
    W->rule->call(W);

  if (W->to_string != NULL)
    i_free(W->to_string);

  W->to_string = node_to_string(W);

  if (W->children != NULL) {
    nn = W->children->nn;
    for (i = 0; i < nn; i++) 
      rule_eval(W->children->Ws[i]);
  }
}

rule *
rule_lookup(const char *name, size_t n, bool is_small)
{
  rule *p;

  p = (rule *) rule_registry; 
  for (; p != NULL && (rule_fp) p->call != NULL; ++p) {
    if ((n == p->n) && 
        (is_small == p->is_small) &&
        (strncmp(name, p->name, MAX_RULE_NAME_SIZE) == 0)) {
      return p;
    }
  }

  return NULL;
}

void
rule_attach(Wht *W, const char *name, int params[], size_t n, bool is_small)
{
  int i;
  rule *p;

  /* Only attempt to apply small rules to smalls and visa versa */
  if ((is_small && W->children != NULL) || ((!is_small) && W->children == NULL))
    return;

  p = rule_lookup(name, n, is_small);

  if (p == NULL) 
    wht_exit("%s was not registered in the rule table", name);

  if (W->rule != NULL)
    rule_free(W->rule);

  W->rule = rule_copy(p);

  for (i = 0; i < MAX_RULE_PARAMS; i++)
    W->rule->params[i] = params[i];
}

void
rule_attach_recursive(Wht *W, const char *name, int params[], size_t n, bool is_small)
{
  int i;

  if ((strcmp(W->rule->name, "small") == 0) || (strcmp(W->rule->name, "split") == 0))
    rule_attach(W, name, params, n, is_small);

  if (W->children == NULL)
    return;

  for (i = 0; i < W->children->nn; i++)
    rule_attach_recursive(W->children->Ws[i], name, params, n, is_small);
}

void
rule_attach_undo(Wht *W)
{
  if (W->error_msg != NULL)
    i_free(W->error_msg);

  if (W->rule != NULL)
    rule_free(W->rule);

  W->error_msg  = NULL;

  if (W->children == NULL) {
    W->rule       = rule_init("small");
    W->apply      = codelet_apply_lookup(W);
  } else {
    W->rule       = rule_init("split");
    W->apply      = split_apply;
  }

}

void
rule_attach_undo_recursive(Wht *W)
{
  int i;

  if (W->error_msg != NULL) 
    rule_attach_undo(W);

  if (W->children != NULL)
    for (i = 0; i < W->children->nn; i++)
      rule_attach_undo_recursive(W->children->Ws[i]);

  W->to_string = node_to_string(W);
}

void
rule_eval_from_string(Wht *W, char *rule)
{
  Wht *R;
  R = parse(rule);

  char   *name;
  int    *params;
  size_t  n;
  bool    is_small;

  name       = R->rule->name;
  params      = R->rule->params;
  n           = R->rule->n;
  is_small    = (bool) (R->children == NULL);

  /* Backtrack any previous errors */
  rule_attach_undo_recursive(W);
  /* Attach rule to all nodes*/
  rule_attach_recursive(W, name, params, n, is_small);
  /* Eval rules */
  rule_eval(W);
  /* Backtrack when error occurs */
  rule_attach_undo_recursive(W);

  wht_free(R);
}

