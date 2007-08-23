%{
#include "wht.h"

#define WHT_NUMBER    1
#define WHT_CODELET   2

union {
  char ident[CODELET_NAME_MAX_SIZE];
  int  value;
} wht_token;

%}

%%
[ \t\n] ;
"["         { return yytext[0];   }
"]"         { return yytext[0];   }
","         { return yytext[0];   }
"_"         { return yytext[0];   }
[0-9]+      { 
              wht_token.value = atoi(yytext);
              return WHT_NUMBER;  
            }
[a-zA-Z]+   { 
              strncpy(wht_token.ident,yytext,CODELET_NAME_MAX_SIZE);
              return WHT_CODELET;
            }
.           { return yytext[0];   }
%%

int
yywrap()
{
  return -1;
}

int
wht_read_int()
{
  if (yylex() != WHT_NUMBER) 
    wht_error("expected int.\n");

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
  unput(x);

  return (x == c);
}

