#include "wht.h"

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

  return W;  
}

