#include "wht.h"

bool
null_accept(Wht *W)
{
  return false; /* Disallow null nodes in plan */
}

void 
null_free(Wht *W) 
{
  if (W->name != NULL)
    i_free(W->name);

  if (W->params != NULL)
    i_free(W->params);

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

  size_t bufsize, n, i;
  char *buf;
  char tmp[DIGIT_SIZE];

  n = W->np;
  if (n < 1) {
    bufsize = IDENT_SIZE + DIGIT_SIZE + 3; /*i.e. IDENT [ DIGIT ] \0*/
    buf = i_malloc(sizeof(char) * bufsize);
    snprintf(buf, bufsize, "%s[%d]", W->name, W->n);
  } else {
    bufsize = IDENT_SIZE + DIGIT_SIZE + 5 + (n-1) + DIGIT_SIZE*(n-1); 
      /*i.e. IDENT ( ) [ DIGIT ] \0 + n-1 COMMAS + n DIGITS*/
    buf = i_malloc(sizeof(char) * bufsize);
    snprintf(buf, IDENT_SIZE + 1, "%s(", W->name);

    for (i = 0; i < n; i++) {
      snprintf(tmp, DIGIT_SIZE, "%d", W->params[i]);
      strncat(buf, tmp, strlen(tmp));

      if (i != n - 1)
        strncat(buf, ",", 1);
    }

    strncat(buf, ")[", 2);
    snprintf(tmp, DIGIT_SIZE, "%d", W->n);
    strncat(buf, tmp, strlen(tmp));
    strncat(buf, "]", 1);
  }

  return buf;
}


Wht *
null_init(char *name, size_t n, int params[], size_t np)
{
  Wht *W;
  size_t i;

  W            = (Wht *) i_malloc(sizeof(Wht));
  W->N         = (1 << n); 
  W->n         = n;
  W->name      = strdup(name);
  W->np        = np;
  W->free      = null_free;
  W->apply     = null_apply;
  W->accept    = null_accept;
  W->to_string = null_to_string;
  W->left      = W->N;
  W->right     = 1;
  W->parent    = NULL;
  W->children  = NULL;

  /** \todo assert np <= MAX_CODELET_PARAMS */

  W->params    = (int *) i_malloc(sizeof(int) * MAX_CODELET_PARAMS);

  for (i = 0; i < np; i++)
    W->params[i] = params[i];

  for (i = 0; i < MAX_ATTRIBUTES; i++)
    W->attr[i] = -1;

  return W;  
}

