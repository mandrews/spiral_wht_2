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
"_"         { return yytext[0];   }
[0-9]+      { 
              yylval.value = atoi(yytext);
              return NUMBER;  
            }
"split"     { return SPLIT; }
"small"     { return SMALL; }
"smallil"   { return SMALLIL; }
.           { return yytext[0];   }
%%

