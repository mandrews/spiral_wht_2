%{

#include "wht.h"

/*
 * Forward declarations for dispatch codelets
 */
Wht *
wht_init_split(Wht *Ws[], size_t nn, int params[], size_t np);

Wht *
wht_init_small(size_t n, int params[], size_t np);

Wht *
wht_init_interleave(size_t n, int params[], size_t np);

Wht *
wht_init_interleave_vector(size_t n, int params[], size_t np);

Wht *
wht_init_right_vector(size_t n, int params[], size_t np);

/*
 * NOTE: Bison does not like the 
 * struct { ... } array;  syntax
 */

struct array
{
  struct wht *values[SPLIT_MAX_FACTORS];
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
    int p[] = {};
    $$ = wht_init_split($3->values, $3->size, p, 0);
  }
  ;

small:
    SMALL '[' NUMBER ']'
  {
    int p[] = {};
    $$ = wht_init_small($3, p, 0);
  }
  ;

smallil:
    SMALLIL '(' NUMBER ')' '[' NUMBER ']'
  {
    int p[] = { $3 };
    $$ = wht_init_interleave($6, p, 1);
  }
  ;

smallv:
    SMALLV '(' NUMBER ')' '[' NUMBER ']'
  {
    int p[] = { $3 };
    $$ = wht_init_right_vector($6, p, 1);
  }
  | SMALLV '(' NUMBER ',' NUMBER ')' '[' NUMBER ']'
  {
    int p[] = { $3, $5 };
    $$ = wht_init_interleave_vector($8, p, 2);
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

  if (p->size > SPLIT_MAX_FACTORS)
    wht_error("Codelet parameters exheeded maximum.");

  return p;
}

