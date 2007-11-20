#include "wht.h"
#include "registry.h"
#include "codelets.h"

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

  W->transform(W);

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

  fprintf(fd, "name:         %-s\n",   W->name);
  fprintf(fd, "N:            %-d\n",   W->N);
  fprintf(fd, "n:            %-d\n",   W->n);
  fprintf(fd, "left:         %-zd\n",  W->left);
  fprintf(fd, "right:        %-zd\n",  W->right);
  fprintf(fd, "self:         %-p\n",  &W);
  fprintf(fd, "parent:       %-p\n",   W->parent);
  fprintf(fd, "apply:        %-p\n",   W->apply);
  fprintf(fd, "free:         %-p\n",   W->free);
  fprintf(fd, "transform:    %-p\n",   W->transform);
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

codelet_transform_fp 
codelet_transform_lookup(const char *name, size_t params)
{
  codelet_transform_entry *p;

  for (p = (codelet_transform_entry *) codelet_transforms_registry; p != NULL && (codelet_transform_fp) p->call != NULL; ++p)
    if ((strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) && (params == p->params))
      return p->call;

  return NULL;
}

void
codelet_transform(Wht *W, const char *name, int params[], size_t n)
{
  codelet_transform_fp f;
  int i;

  f = codelet_transform_lookup(name, n);

  if (f == NULL)
    wht_exit("%s was not registered in the transform table", name);

  W->transform = f;

  for (i = 0; i < n; i++)
    W->params[i] = params[i];

  i_free(W->name);

  if (n > 0)
    W->name = append_params_to_name(name, params, n);
  else
    W->name = strdup(name);
}

void
codelet_transform_recursive(Wht *W, const char *name, int params[], size_t n)
{
  int i;

  if ((strcmp(W->name, "small") == 0) || (strcmp(W->name, "split") == 0))
    codelet_transform(W, name, params, n);

  if (W->children == NULL)
    return;

  for (i = 0; i < W->children->nn; i++)
    codelet_transform_recursive(W->children->Ws[i], name, params, n);
}

void
codelet_transform_undo(Wht *W)
{
  if (W->name != NULL)
    i_free(W->name);

  if (W->error_msg != NULL)
    i_free(W->error_msg);

  if (W->children == NULL) {
    W->name       = strdup("small");
    W->apply      = codelet_apply_lookup(W->n, "small", NULL, 0);
    W->transform  = null_transform;
    W->error_msg  = NULL;
  } else {
    W->name       = strdup("split");
    W->apply      = split_apply;
    W->transform  = split_transform;
    W->error_msg  = NULL;
  }
}

void
codelet_transform_undo_recursive(Wht *W)
{
  int i;

  if (W->error_msg != NULL) 
    codelet_transform_undo(W);

  if (W->children == NULL)
    return;

  for (i = 0; i < W->children->nn; i++)
    codelet_transform_undo_recursive(W->children->Ws[i]);
}

void
transform(Wht *W, const char *name, int params[], size_t n)
{
  /* Tag with transform function */
  codelet_transform_recursive(W, name, params, n);
  /* Apply transform function */
  W->transform(W);
  /* Backtrack when error occurs */
  codelet_transform_undo_recursive(W);
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

  tmp = W->to_string(W);

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
append_params_to_name(const char *name, int params[], size_t n)
{
  const size_t DIGIT_SIZE = 32;
  size_t bufsize;
  int i;
  char *buf;
  char tmp[DIGIT_SIZE];

  bufsize = strlen(name) + DIGIT_SIZE*n + (n-1) + 3; 
   /* n DIGITS + (n-1) COMMAS + '(' + ')' + '\0' */

  buf = i_malloc(sizeof(char) * bufsize);

  buf[0] = '\0';
  strncat(buf, name, strlen(name));
  strncat(buf, "(", 1);

  for (i = 0; i < n; i++) {
    snprintf(tmp, DIGIT_SIZE, "%d", params[i]);
    strncat(buf, tmp, strlen(tmp));

    if (i != n - 1)
      strncat(buf, ",", 1);
  }

  strncat(buf, ")", 1);

  return buf;
}

