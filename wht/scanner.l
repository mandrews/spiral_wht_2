%{

#include "parser.h"

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
"split"     { return SPLIT;       }
"small"     { return SMALL;       }
"smallil"   { return SMALLIL;     }
"smallv"    { return SMALLV;      }
.           { return yytext[0];   }
%%

