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

  rule_apply(W);

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
  fprintf(fd, "rule:         %-p\n",   W->rule);
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

rule_data *
rule_data_init()
{
  size_t i;
  rule_data *rule;

  rule = i_malloc(sizeof(*rule));

  rule->ident[0] = '\0';

  rule->n = 0;

  for (i = 0; i < MAX_RULE_PARAMS; i++)
    rule->params[i] = UNSET_PARAMETER;

  rule->call = NULL;

  return rule;
}

rule_data *
rule_data_copy(rule_data *src)
{
  int i;
  rule_data *dst;

  dst = rule_data_init();

  dst->n        = src->n;
  dst->is_small = src->is_small;
  dst->call     = src->call;

  strcpy(dst->ident, src->ident);

  for (i = 0; i < MAX_RULE_PARAMS; i++)
    dst->params[i] = src->params[i];

  return dst;
}

void
rule_data_free(rule_data *rule)
{
  i_free(rule);
}

void
rule_apply(Wht *W)
{
  if (W->children == NULL) {
    if (W->rule->call != NULL)
      return W->rule->call(W);
    else
      return;
  }

  size_t i, nn;

  nn = W->children->nn;

  if (W->rule->call != NULL)
    W->rule->call(W);

  for (i = 0; i < nn; i++) 
    rule_apply(W->children->Ws[i]);
}

rule_data *
rule_lookup(const char *ident, size_t n, bool is_small)
{
  rule_data *p;

  p = (rule_data *) rule_registry; 
  for (; p != NULL && (rule_fp) p->call != NULL; ++p) {
    if ((n == p->n) && 
        (is_small == p->is_small) &&
        (strncmp(ident, p->ident, MAX_RULE_IDENT_SIZE) == 0)) {
      return p;
    }
  }

  return NULL;
}

void
rule_attach(Wht *W, const char *ident, int params[], size_t n, bool is_small)
{
  int i;
  rule_data *p;

  /* Only attempt to apply small rules to smalls and visa versa */
  if ((is_small && W->children != NULL) || ((!is_small) && W->children == NULL))
    return;

  p = rule_lookup(ident, n, is_small);

  if (p == NULL) 
    wht_exit("%s was not registered in the rule table", ident);

  W->rule = rule_data_copy(p);

  for (i = 0; i < MAX_RULE_PARAMS; i++)
    W->rule->params[i] = params[i];

  W->name = strdup(ident);
}

void
rule_attach_recursive(Wht *W, const char *ident, int params[], size_t n, bool is_small)
{
  int i;

  if ((strcmp(W->name, "small") == 0) || (strcmp(W->name, "split") == 0))
    rule_attach(W, ident, params, n, is_small);

  if (W->children == NULL)
    return;

  for (i = 0; i < W->children->nn; i++)
    rule_attach_recursive(W->children->Ws[i], ident, params, n, is_small);
}

void
rule_attach_undo(Wht *W)
{
  if (W->name != NULL)
    i_free(W->name);

  if (W->error_msg != NULL)
    i_free(W->error_msg);

  rule_data_free(W->rule);

  W->error_msg  = NULL;

  W->rule       = rule_data_init();

  if (W->children == NULL) {
    W->name       = strdup("small");
    W->apply      = codelet_apply_lookup(W);
  } else {
    W->name       = strdup("split");
    W->apply      = split_apply;
  }
}

void
rule_attach_undo_recursive(Wht *W)
{
  int i;

  if (W->error_msg != NULL) 
    rule_attach_undo(W);

  if (W->children == NULL)
    return;

  for (i = 0; i < W->children->nn; i++)
    rule_attach_undo_recursive(W->children->Ws[i]);
}

void
rule_apply_from_string(Wht *W, char *transform)
{
  Wht *T;
  T = parse(transform);

  char   *ident;
  int    *params;
  size_t  n;
  bool    is_small;

  ident       = T->rule->ident;
  params      = T->rule->params;
  n           = T->rule->n;
  is_small    = (bool) (T->children == NULL);

  /* Backtrack any previous errors */
  rule_attach_undo_recursive(W);
  /* Tag with transform function */
  rule_attach_recursive(W, ident, params, n, is_small);
  /* Apply transform function */
  rule_apply(W);
  /* Backtrack when error occurs */
  rule_attach_undo_recursive(W);

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

