%{

#include "parser.h"
#include <string.h>

%}

%option header-file="scanner.h"
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
              yylval.ident = strdup(yytext);
              return IDENT;  
            }
.           { return yytext[0];   }
%%

