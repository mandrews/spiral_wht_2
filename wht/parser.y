%{

#include "wht.h"

/*
 * Forward declarations for dispatch codelets
 */
Wht *
wht_init_split(Wht *Ws[], size_t nn);

Wht *
wht_init_small(size_t n);

Wht *
wht_init_interleave(size_t n, size_t k);

Wht *
wht_init_right_vector(size_t n, size_t v);

Wht *
wht_init_interleave_vector(size_t n, size_t v, size_t k);

#define MAX_SPLIT_SIZE 40

/*
 * NOTE: Bison does not like the 
 * struct { ... } array;  syntax
 */

struct array
{
  struct wht *values[MAX_SPLIT_SIZE];
  size_t size;
};

struct array *
array_append(struct wht *x, struct array *p);

Wht *wht_root;

%}

%union {
  int  value; 
  char ident;
  struct wht *W;
  struct array *Ws;
}

%token <value> NUMBER
%token <ident> SPLIT 
%token <ident> SMALL 
%token <ident> SMALLIL
%token <ident> SMALLV

%type <W>  node
%type <W>  split
%type <W>  small
%type <W>  smallil
%type <W>  smallv

%type <Ws> nodes 

%start node

%%

node:
    split   { wht_root = $1; $$ = $1; }
  | small   { wht_root = $1; $$ = $1; }
  | smallil { wht_root = $1; $$ = $1; }
  | smallv  { wht_root = $1; $$ = $1; }
  ;

split:
    SPLIT '[' nodes ']'
  {
    $$ = wht_init_split($3->values, $3->size);
  }
  ;

small:
    SMALL '[' NUMBER ']'
  {
    $$ = wht_init_small($3);
  }
  ;

smallil:
    SMALLIL '(' NUMBER ')' '[' NUMBER ']'
  {
    $$ = wht_init_interleave($6,$3);
  }
  ;

smallv:
    SMALLV '(' NUMBER ')' '[' NUMBER ']'
  {
    $$ = wht_init_right_vector($6,$3);
  }
  | SMALLV '(' NUMBER ',' NUMBER ')' '[' NUMBER ']'
  {
    $$ = wht_init_interleave_vector($8,$3,$5);
  }
  ;

nodes:
    node ',' nodes
  {
    $$ = array_append($1,$3);
  }
  | node
  {
    $$ = array_append($1,NULL);
  }
  ;
%%

int
yyerror(const char *s)
{
  wht_error(s);
}

struct array *
array_append(struct wht *x, struct array *p)
{
  if (p == NULL) {
    p = wht_malloc(sizeof(*p));
    p->size = 0;
  }

  p->values[p->size] = x;
  p->size++;

  if (p->size > MAX_SPLIT_SIZE)
    wht_error("Codelet parameters exheeded maximum.");

  return p;
}

