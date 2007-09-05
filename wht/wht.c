#include "wht.h"

#if 0
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
#endif

#if 0
Wht *
wht_parse_split()
{
  Wht *Ws[SPLIT_MAX_FACTORS];
  int nn;

  if (wht_is_codelet("split")) {
    wht_require('[');
    nn = 0;
    Ws[0] = wht_parse_next();
    nn++;
    while (wht_check(',')) {
      if (nn == SPLIT_MAX_FACTORS)
        wht_error("too many arguments for split[ ] in wht_parse()");
      wht_require(',');
      Ws[nn] = wht_parse_next();
      nn++;
    }
    wht_require(']');

    return wht_init_split(nn, Ws);
  }

  return NULL;
}
#endif

extern Wht *wht_root;

Wht *
wht_parse(char *in)
{
  Wht *wht;

  yy_scan_string(in);
  yyparse();
  wht = wht_root;

  return wht;
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
  W->nILNumber = 1;

  return W;  
}

void 
wht_free_codelet(Wht *W) 
{
  wht_free(W);
}

