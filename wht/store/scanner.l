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
"null"              { return NULL_TOKEN;  }
[a-zA-Z0-9]+        { 
                      // yylval.c_str_type = strdup(yytext);
                      yylval.c_str_type = yytext;
                      return STRING_TOKEN;  
                    }
.                   { return yytext[0];   }
%%

