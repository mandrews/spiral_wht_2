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
  fprintf(fd, "free:         %-p\n",   W->free);
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
  /* NOTE: n should log[10](length of maximum integer)
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

  tmp = to_string(W);

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

char *
params_to_string(Wht *W)
{
  /**
   * \todo Take log10 of INT_MAX determine length of integer
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

char *
to_string(Wht *W)
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
    tmp  = to_string(W->children->Ws[j]);
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

