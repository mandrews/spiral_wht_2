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
 * \file node.c
 * \author Michael Andrews
 *
 * \brief Implementation of WHT node independent functions.
 *
 */

#include "wht.h"
#include "registry.h"

/**
 * \note This symbol cannot be inlined since it needs an address to be referenced
 */
void
null_apply(Wht *W, long S, size_t U, wht_value *x)
{
  /* Empty */
}

Wht *
node_init(size_t n, char *name)
{
  Wht *W;
  size_t i;

  W            = (Wht *) i_malloc(sizeof(Wht));
  W->N         = (1 << n); 
  W->n         = n;
  W->apply     = null_apply;
  W->error_msg = NULL;
  W->left      = W->N;
  W->right     = 1;
  W->parent    = NULL;
  W->children  = NULL;
  W->to_string = NULL;
  W->rule      = rule_init(name);

  for (i = 0; i < MAX_ATTRIBUTES; i++)
    W->attr[i] = UNSET_ATTRIBUTE;

  for (i = 0; i < MAX_ATTRIBUTES; i++) {
    W->provides[i] = false;
    W->requires[i] = false;
  }

  return W;  
}

void 
node_free(Wht *W) 
{
  int i;

  if (W->children != NULL) {
    for (i = 0; i < W->children->nn; i++) 
      node_free(W->children->Ws[i]);

    i_free(W->children);
  }

  if (W->to_string != NULL)
    i_free(W->to_string);

  if (W->error_msg != NULL)
    i_free(W->error_msg);

  if (W->rule != NULL)
    rule_free(W->rule);

  i_free(W);
}


char *
params_to_string(Wht *W)
{
  /**
   * \todo Use i_itoa to calculate DIGIT_SIZE
   */
  const size_t DIGIT_SIZE = 32; 

  size_t bufsize, n;
  int i;
  char *buf;
  char tmp[DIGIT_SIZE];

  n = W->rule->n;

  bufsize = DIGIT_SIZE*n + (n-1) + 1; 
   /* n DIGITS + (n-1) COMMAS + '\0' */

  buf = i_malloc(sizeof(char) * bufsize);

  buf[0] = '\0';

  for (i = 0; i < n; i++) {
    snprintf(tmp, DIGIT_SIZE, "%d", W->rule->params[i]);
    strncat(buf, tmp, strlen(tmp));

    if (i != n - 1)
      strncat(buf, ",", 1);
  }

  return buf;
}


/** \todo Fix memory leak @ name_to_string:151 */
char *
name_to_string(Wht *W)
{
  char *buf, *tmp;
  size_t len;

  len = strlen(W->rule->name) + 1; 
  buf = i_malloc(sizeof(char) * len);
  snprintf(buf, len, "%s", W->rule->name);

  if (W->rule != NULL && W->rule->n > 0) {
    tmp  = params_to_string(W);
    len += strlen(tmp) + 3; /* ( .. ) \0 */
    buf  = realloc(buf, sizeof(char) * len);
    strncat(buf,"(",1);
    strncat(buf, tmp, strlen(tmp) + 1);
    strncat(buf,")",1);

    i_free(tmp);
  }

  if (W->children == NULL) {
    tmp  = i_itoa(W->n);
    len += strlen(tmp) + 3; /* ( .. ) \0 */
    buf  = realloc(buf, sizeof(char) * len);
    strncat(buf,"[",1);
    strncat(buf, tmp, strlen(tmp) + 1);
    strncat(buf,"]",1);

    i_free(tmp);
  }

  return buf;
}

/** \todo Fix memory leak @ node_to_string:186 */
char *
node_to_string(Wht *W)
{
  char *buf, *tmp;
  size_t nn, i, j, len;
  
  buf = name_to_string(W);

  if (W->children == NULL)
    return buf;

  len = strlen(buf) + 3; /* [ .. ] \0 */
  buf = realloc(buf, sizeof(char) * len);

  strncat(buf,"[",1);

  nn = W->children->nn;

  /* Iterate over children WHTs, stored anti lexigraphically  */
  for (i = 0; i < nn; i++) {
    j    = nn - i - 1;
    tmp  = node_to_string(W->children->Ws[j]);
    len += strlen(tmp) + 2; /* , \0*/
    buf  = realloc(buf, sizeof(char) * len);

    strncat(buf, tmp, strlen(tmp) + 1);

    /* Don't add comma for last child */
    if (i < nn - 1)
      strncat(buf, ",", 1);

    i_free(tmp);
  }

  strncat(buf,"]",1);

  return buf;
}

bool
children_require(Wht *W, enum attribute_names A)
{
  int i;

  if (W->children == NULL)
    return W->requires[A];

  for (i = 0; i < W->children->nn; i++)
    if (! children_require(W->children->Ws[i], A))
      return false;

  return true;
}

bool
parent_provides(Wht *W, enum attribute_names A) 
{
  Wht *Wp;

  Wp = W->parent;

  while (Wp != NULL) {
    if (Wp->provides[A])
      return true;

    Wp = Wp->parent;
  }

  return false;
}

void
set_parent_requires(Wht *W, enum attribute_names A) 
{
  Wht *Wp;

  Wp = W->parent;

  while (Wp != NULL) {
    if (! Wp->provides[A])
      Wp->requires[A] = true;
    Wp = Wp->parent;
  }
}

