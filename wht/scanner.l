%{
#include "wht.h"
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
