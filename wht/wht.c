#include "wht.h"

#include "scanner.h"

/* This symbol points to the root of the parsed wht tree */
extern Wht *wht_root;

Wht *
parse(char *in)
{
  Wht *W;
  YY_BUFFER_STATE buf;

  buf = yy_scan_string(in);

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

char * 
i_itoa(int i)
{
  const size_t n = sizeof(int) * 8; 
  /** 
   * \note n should log[10](length of maximum integer)
   * This should work since log[2](x) > log[10](x) 
   */

  char *buf;
  buf = malloc(sizeof(char) * n);

  snprintf(buf, n, "%d", i);

  return buf;
}

void 
error_msg_set(Wht *W, char *format, ...) 
{ 
  va_list ap; 
  char *tmp;

  /* Policy to keep first encountered error message */
  if (W->error_msg != NULL)
    return;

  W->error_msg = i_malloc(sizeof(char) * MAX_MSG_LEN);

  tmp = node_to_string(W);

  va_start(ap, format); 
  vsnprintf(W->error_msg, MAX_MSG_LEN, format, ap); 
  va_end(ap); 

  strcat(W->error_msg, " @ ");
  strcat(W->error_msg, tmp);

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

