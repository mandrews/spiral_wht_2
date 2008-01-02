%{
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "store.h"

/* This symbol points to the finished data structure */
elem *elem_root;

%}

%union {
  struct elem      *elem_type;
  char              *c_str_type;
}

/* union type             token */
%token <c_str_type>       STRING_TOKEN
%token <null_type>        NULL_TOKEN

/* union type             rule */
%type <elem_type>         elem
%type <elem_type>         hash
%type <elem_type>         hash_pairs
%type <elem_type>         hash_pair
%type <elem_type>         list
%type <elem_type>         list_elems
%type <elem_type>         string
%type <elem_type>         null
%type <c_str_type>        key

%start elem 

%%

elem:
    hash
    {
      $$ = $1;
      elem_root = $$;
    }
  | list
    {
      $$ = $1;
      elem_root = $$;
    }
  | string
    {
      $$ = $1;
      elem_root = $$;
    }
  | null
    {
      $$ = $1;
      elem_root = $$;
    }
  ;

hash:
    '{' '}'
    {
      $$ = hash_init();
    }
  | '{' hash_pairs '}'
    {
      $$ = $2;
    }
  ;

hash_pairs:
    hash_pair
    {
      $$ = $1;
    }
  | hash_pairs ',' hash_pair
    {
      $$ = $1;
      hash_merge($1, $3);
      hash_free($3);
    }
  ;

hash_pair:
    key ':' elem
    {
      $$ = hash_init();
      hash_insert($$, $1, $3);
      elem_free($3);
      // free($1);
    }
  ;

key:
    '"' STRING_TOKEN  '"'
    {
      $$ = $2;
    }
  ;

string:
    '"' STRING_TOKEN  '"'
    {
      $$ = string_init($2);
      // free($2);
    }
  ;

list:
    '[' ']'
    {
      $$ = list_init(0);
    }
  | '[' list_elems ']'
    {
      $$ = $2;
    }
  ;

list_elems:
    elem
    {
      $$ = list_init(1);
      list_insert($$, 0, $1);
      elem_free($1);
    }
  | list_elems ',' elem
    {
      list_append($1, $3);
      $$ = $1;
      elem_free($3);
    }
  ;

null:
    NULL_TOKEN
    {
      $$ = null_init();
    }
  ;

%%

int 
yyerror(char *s) 
{
  printf("error: %s\n", s);

  return 1;
}

