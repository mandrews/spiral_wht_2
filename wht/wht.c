#include "wht.h"

#include "scanner.h"

#include "getopt.h"


#if 0
int
yywrap()
{
  return -1;
}
#endif

int
wht_read_int()
{
  if (yylex() != WHT_NUMBER)
    wht_error("expected int.");

  return wht_token.value;
}

void 
wht_next_codelet()
{
  if (yylex() != WHT_CODELET)
    wht_error("expected codelet.");
}

int
wht_is_codelet(char *f) 
{
  return (strncmp(wht_token.ident, f, strlen(f)) == 0);
}

void
wht_require(char c)
{
  if (yylex() != (c))
    wht_error("expected '%c'.", c); 
}

int
wht_check(char c)
{
  char x;
  x = yylex();
  wht_unput(x);

  return (x == c);
}

Wht *
wht_parse_helper()
{
  int i;
  Wht * W;

  W = NULL;

  wht_next_codelet();
  for (i=0; i< REGISTERED_CODELETS;i++) {
    W = (callbacks[i])();
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
  return wht_parse_helper();
}

static void
usage() {
  printf("wht: -w PLAN\n");
  exit(1);
}

int
main (int argc, char **argv)
{
  char *wht_plan;
  int c;

  wht_plan = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "hw:")) != -1)
    switch (c) {
      case 'w':
        wht_plan = optarg;
        break;
      case 'h':
        usage();
      default:
        usage();
    }

  if (wht_plan == NULL)
    usage();

  Wht *W;
  wht_value *x;
  long N;
  int i;

  W = wht_parse(wht_plan);

  N = W->N;

  x = (wht_value *) malloc(N * sizeof(wht_value));
  for (i = 0; i < N; i++)
    x[i] = 0.0;

  wht_apply(W,1,0,x);

  return 0;
}
