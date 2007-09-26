%{
#include "wht.h"
#include "registry.h"

/*
 * NOTE: Bison does not like the 
 * struct { ... } nodes;  syntax
 */

/* 
 * TODO: This repetition could be avoided with void * type values
 */
struct nodes
{
  struct Wht *values[MAX_SPLIT_NODES];
  size_t size;
};

struct params
{
  int    values[MAX_CODELET_PARAMS];
  size_t size;
};

struct Wht* 
parse_split(char *ident, struct nodes *nodes, struct params *params);

struct Wht* 
parse_small(char *ident, size_t size, struct params *params);

struct nodes *
nodes_append(struct nodes *p, struct Wht *x);

struct params *
params_append(struct params *p, int i);

struct params *
empty_params();

/* This symbol points to the root of the parsed wht tree */
struct Wht *wht_root;

%}

%union {
  int  value; 
  char *ident;
  struct Wht *W;
  struct nodes *Ws;
  struct params *Ps;
}

%token <value> NUMBER
%token <ident> IDENT

/*
 * NOTE: This is an entirely different namespace ...
 * node here corresponds to the rule node, not the struct node
 */

%type <W>  node
%type <W>  split
%type <W>  small

%type <Ws> nodes 
%type <Ps> params

/* %destructor { wht_free($$); } IDENT */

%start node

%%

node:
    split   { wht_root = $1; $$ = $1; }
  | small   { wht_root = $1; $$ = $1; }
  ;

split:
    IDENT '[' nodes ']'
  {
    $$ = parse_split($1, $3, empty_params());
  }
  | IDENT '(' params ')' '[' nodes ']'
  {
    $$ = parse_split($1, $6, $3);
  }
  ;

small:
    IDENT '[' NUMBER ']'
  {
    $$ = parse_small($1, $3, empty_params());
  }
  | IDENT '(' params ')' '[' NUMBER ']'
  {
    $$ = parse_small($1, $6, $3);
  }
  ;

/* 
 * TODO: Bison documentation recommends left most recursion,
 * would need to reverse the node list (see params rule)
 */
nodes:
    node ',' nodes
  {
    $$ = nodes_append($3,$1);
  }
  | node
  {
    $$ = nodes_append(NULL, $1);
  }
  ;

params:
    params ',' NUMBER
  {
    $$ = params_append($1,$3);
  }
  | NUMBER
  {
    $$ = params_append(NULL, $1);
  }
  ;
%%

int
yyerror(const char *s)
{
  wht_error(s);
}

struct Wht* 
parse_split(char *ident, struct nodes *nodes, struct params *params)
{
  split_init_fp call;
 
  call  = lookup_split(ident, params->size);

  if (call == NULL)
    wht_error("%s was not registered in the split table", ident);

  return (call)(ident, nodes->values, nodes->size, params->values, params->size);
}

struct Wht* 
parse_small(char *ident, size_t size, struct params *params)
{
  small_init_fp call;
 
  call  = lookup_small(ident, params->size);

  if (call == NULL)
    wht_error("%s was not registered in the small table", ident);

  return (call)(ident, size, params->values, params->size);
}

struct nodes *
nodes_append(struct nodes *p, struct Wht *x)
{
  if (p == NULL) {
    p = wht_malloc(sizeof(*p));
    p->size = 0;
  }

  p->values[p->size] = x;
  p->size++;

  if (p->size > MAX_SPLIT_NODES)
    wht_error("number of codelets exheeded maximum.");

  return p;
}

struct params *
params_append(struct params *p, int x)
{
  if (p == NULL) 
    p = empty_params();

  p->values[p->size] = x;
  p->size++;

  if (p->size > MAX_CODELET_PARAMS)
    wht_error("codelet parameters exheeded maximum.");

  return p;
}

struct params *
empty_params() 
{
  struct params *p;

  p = wht_malloc(sizeof(*p));
  p->size = 0;

  return p;
}

