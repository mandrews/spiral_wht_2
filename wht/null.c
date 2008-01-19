#include "wht.h"

void 
null_free(Wht *W) 
{
  if (W->name != NULL)
    i_free(W->name);

  if (W->error_msg != NULL)
    i_free(W->error_msg);

  if (W->rule != NULL)
    rule_free(W->rule);

  i_free(W);
}

void
null_apply(Wht *W, long S, size_t D, wht_value *x)
{
  /* Empty */
}

Wht *
null_init(size_t n, char *name)
{
  Wht *W;
  size_t i;

  W            = (Wht *) i_malloc(sizeof(Wht));
  W->N         = (1 << n); 
  W->n         = n;
  W->name      = strdup(name);
  W->free      = null_free;
  W->apply     = null_apply;
  W->error_msg = NULL;
  W->left      = W->N;
  W->right     = 1;
  W->parent    = NULL;
  W->children  = NULL;
  W->rule      = rule_init();
  strcpy(W->rule->ident, name);

  for (i = 0; i < MAX_ATTRIBUTES; i++)
    W->attr[i] = UNSET_ATTRIBUTE;

  return W;  
}

