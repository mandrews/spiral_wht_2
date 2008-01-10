#include "wht.h"

void
null_transform(Wht *W)
{
  /* Empty */
}

void 
null_free(Wht *W) 
{
  if (W->name != NULL)
    i_free(W->name);

  if (W->error_msg != NULL)
    i_free(W->error_msg);

  i_free(W);
}

void
null_apply(Wht *W, long S, size_t D, wht_value *x)
{
  /* Empty */
}


char *
null_to_string(Wht *W)
{
  /**
   * \todo Take log10 of INT_MAX determine length of integer
   */
  const size_t DIGIT_SIZE = 32;
  const size_t IDENT_SIZE = strlen(W->name) + 1;

  size_t bufsize;
  char *buf;
  char *params;

  if (W->n_params == 0) {
    bufsize = IDENT_SIZE + DIGIT_SIZE + 3; /*i.e. IDENT [ DIGIT ] \0*/
    buf = i_malloc(sizeof(char) * bufsize);
    snprintf(buf, bufsize, "%s[%d]", W->name, W->n);
  } else {
    params = params_to_string(W->params, W->n_params);
    bufsize = IDENT_SIZE + DIGIT_SIZE + 5 + strlen(params); /*i.e. IDENT (...) [ DIGIT ] \0*/
    buf = i_malloc(sizeof(char) * bufsize);
    snprintf(buf, bufsize, "%s(%s)[%d]", W->name, params, W->n);
    i_free(params);
  }

  return buf;
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
  W->transform = null_transform;
  W->to_string = null_to_string;
  W->error_msg = NULL;
  W->left      = W->N;
  W->right     = 1;
  W->parent    = NULL;
  W->children  = NULL;
  W->n_params  = 0;

  for (i = 0; i < MAX_ATTRIBUTES; i++)
    W->attr[i] = UNSET_ATTRIBUTE;

  for (i = 0; i < MAX_CODELET_PARAMS; i++)
    W->params[i] = UNSET_PARAMETER;

  return W;  
}

