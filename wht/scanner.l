%{

#include "parser.h"
#include "strings.h"

%}

%option noyywrap nounput

%%
[ \t\n] ;
"["         { return yytext[0];   }
"]"         { return yytext[0];   }
"("         { return yytext[0];   }
")"         { return yytext[0];   }
","         { return yytext[0];   }
[0-9]+      { 
              yylval.value = atoi(yytext);
              return NUMBER;  
            }
[a-zA-Z]+   { 
              yylval.ident = strndup(yytext, strlen(yytext));
              return IDENT;  
            }
.           { return yytext[0];   }
%%

