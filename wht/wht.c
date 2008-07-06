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
 * \file wht.c
 * \author Michael Andrews
 *
 * \brief Implementation of misc WHT plan functions.
 */

#include "wht.h"

#include "scanner.h"

/**
 * \var wht_root 
 * \brief This symbol points to the root of the parsed wht tree 
 */
extern Wht *wht_root; 

Wht *
parse(char *s)
{
  Wht *W;
  YY_BUFFER_STATE buf;

  buf = yy_scan_string(s);

  yyparse();

  W = wht_root;

  rule_eval(W);

  yy_delete_buffer(buf);

  return W;
}

void
info(void)
{
  printf("CC: %s\n", WHT_CC);
  printf("CFLAGS: %s\n", WHT_CFLAGS);
  printf("type: %s\n", WHT_TYPE_STRING);
  printf("max_unroll: %d\n", WHT_MAX_UNROLL);
  printf("max_interleave: %d\n", WHT_MAX_INTERLEAVE);
  printf("vector_size: %d\n", WHT_VECTOR_SIZE);
}

void
dump(Wht *W, FILE *fd)
{
  int i;

  fprintf(fd, "N:            %-d\n",   W->N);
  fprintf(fd, "n:            %-d\n",   W->n);
  fprintf(fd, "left:         %-zd\n",  W->left);
  fprintf(fd, "right:        %-zd\n",  W->right);
  fprintf(fd, "self:         %-p\n",  &W);
  fprintf(fd, "parent:       %-p\n",   W->parent);
  fprintf(fd, "apply:        %-p\n",   W->apply);
  fprintf(fd, "name:         %-p\n",   W->rule->name);
  fprintf(fd, "to_string:    %-p\n",   W->to_string); 
  fprintf(fd, "error_msg:    %-s\n",   W->error_msg);
  fprintf(fd, "\n");

  if (W->children == NULL) 
    return;

  fprintf(fd, "nn:           %-d\n",   W->children->nn);

  fprintf(fd, "children:\n");
  for (i = 0; i < W->children->nn; i++)
    dump(W->children->Ws[i], fd);
}

void 
error_msg_set(Wht *W, char *format, ...) 
{ 
  va_list ap; 
  char *tmp;
  size_t max;

  /* Policy to keep first encountered error message */
  if (W->error_msg != NULL)
    return;

  W->error_msg = i_malloc(sizeof(char) * MAX_MSG_LEN);

  tmp = node_to_string(W);

  va_start(ap, format); 
  vsnprintf(W->error_msg, MAX_MSG_LEN, format, ap); 
  va_end(ap); 

  strcat(W->error_msg, " @ ");

  max = MAX_MSG_LEN - strlen(W->error_msg) - 1;

  strncat(W->error_msg, tmp, max);

  free(tmp);
}

char *
error_msg_get(Wht *W)
{ 
  if (W->error_msg != NULL)
    return W->error_msg;

  if (W->children != NULL) {
    size_t i, nn;
    char *tmp;

    nn = W->children->nn;

    for (i = 0; i < nn; i++) {
      tmp = error_msg_get(W->children->Ws[i]);

      if (tmp != NULL)
        return tmp;
    }
  }

  return NULL;
}

wht_value *
random_vector(size_t N)
{
  wht_value *x;
  int i;

  srandom((unsigned int) (getpid() * M_PI));
  
  x = (wht_value *) i_malloc(N * sizeof(wht_value));
  for (i = 0; i < N; i++) 
    x[i] = ((wht_value) (rand() + 1.0L))/RAND_MAX;

  return x;
}

#define pkg_max(a,b) (a > b ? a : b)

wht_value
max_norm(const wht_value *x, const wht_value *y, size_t N)
{
  int i;
  wht_value norm;

  norm = WHT_STABILITY_THRESHOLD*1e-1;
#if   WHT_FLOAT==1
  for (i = 0; i < N; i++) 
    norm = pkg_max(fabsf(x[i] - y[i]), norm);
#endif/*WHT_FLOAT*/

#if   WHT_DOUBLE==1
  for (i = 0; i < N; i++) 
    norm = pkg_max(fabs(x[i] - y[i]), norm);
#endif/*WHT_DOUBLE*/

  return norm;

}

#undef pkg_max

