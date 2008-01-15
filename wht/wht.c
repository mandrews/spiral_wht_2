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

codelet_transform_fp 
codelet_transform_lookup(const char *name, size_t params, bool small)
{
  codelet_transform_entry *p;

  p = (codelet_transform_entry *) codelet_transforms_registry; 
  for (; p != NULL && (codelet_transform_fp) p->call != NULL; ++p)
    if ((strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) && (params == p->params) && (small == p->small))
      return p->call;

  return NULL;
}

void
codelet_transform(Wht *W, const char *name, int params[], size_t n, bool small)
{
  codelet_transform_fp f;
  int i;

  /* Only attempt to apply small transforms to smalls and visa versa */
  if ((small && W->children != NULL) || ((!small) && W->children == NULL))
    return;

  f = codelet_transform_lookup(name, n, small);

  if (f == NULL) 
    wht_exit("%s was not registered in the transform table", name);

  W->transform = f;
  W->n_params = n;

  for (i = 0; i < n; i++)
    W->params[i] = params[i];

  i_free(W->name);

  W->name = strdup(name);
}

void
codelet_transform_recursive(Wht *W, const char *name, int params[], size_t n, bool small)
{
  int i;

  if ((strcmp(W->name, "small") == 0) || (strcmp(W->name, "split") == 0))
    codelet_transform(W, name, params, n, small);

  if (W->children == NULL)
    return;

  for (i = 0; i < W->children->nn; i++)
    codelet_transform_recursive(W->children->Ws[i], name, params, n, small);
}

void
codelet_transform_undo(Wht *W)
{
  if (W->name != NULL)
    i_free(W->name);

  if (W->error_msg != NULL)
    i_free(W->error_msg);

  W->n_params   = 0; /* XXX: Reset params */
  W->error_msg  = NULL;

  if (W->children == NULL) {
    W->name       = strdup("small");
    W->apply      = codelet_apply_lookup(W);
    W->transform  = null_transform;
  } else {
    W->name       = strdup("split");
    W->apply      = split_apply;
    W->transform  = split_transform;
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
transform_from_string(Wht *W, char *transform)
{
  Wht *T;
  T = parse(transform);

  char   *name;
  int    *params;
  size_t  n;
  bool    small;

  name    = T->name;
  params  = T->params;
  n       = T->n_params;
  small   = (bool) (T->children == NULL);

  /* Backtrack any previous errors */
  codelet_transform_undo_recursive(W);
  /* Tag with transform function */
  codelet_transform_recursive(W, name, params, n, small);
  /* Apply transform function */
  W->transform(W);
  /* Backtrack when error occurs */
  codelet_transform_undo_recursive(W);

  wht_free(T);
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

