#include "wht.h"

Wht *
wht_parse_next()
{
  int i;
  Wht * W;

  W = NULL;

  wht_next_codelet();
  for (i=0; i< REGISTERED_CODELETS;i++) {
    W = (codelets[i])();
    if (W) 
      return W;
  }

  if (!W) 
    wht_error("Did not match any codelet.");

  return NULL;
}

Wht *
wht_parse(char *in)
{
  yy_scan_string(in);
  return wht_parse_next();
}

Wht *
wht_init_codelet(int n)
{
  Wht *W;

  W            = (Wht *) wht_malloc(sizeof(Wht));
  W->N         = (long) pow((double) 2, (double) n);
  W->n         = n;
  W->free      = wht_free_codelet;
  W->apply     = NULL;

  return W;  
}

void 
wht_free_codelet(Wht *W) 
{
  wht_free(W);
}

