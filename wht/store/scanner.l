%{
#include <string.h>

#include "parser.h"

%}

%option noyywrap nounput

%%
[ \t\n] ;
"{"                 { return yytext[0];   }
"}"                 { return yytext[0];   }
"["                 { return yytext[0];   }
"]"                 { return yytext[0];   }
":"                 { return yytext[0];   }
","                 { return yytext[0];   }
"\""                { return yytext[0];   }
"null"              { return NIL_TOKEN;  }
[a-zA-Z0-9]+        { 
                      // yylval.cstr_type = strdup(yytext);
                      yylval.cstr_type = yytext;
                      return TEXT_TOKEN;  
                    }
.                   { return yytext[0];   }
%%

