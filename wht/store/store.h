#ifndef HASH_H
#define HASH_H

#include "uthash.h"

#include <stdbool.h>

typedef struct elem elem;
typedef struct pair pair;

typedef elem text;
typedef elem hash;
typedef elem list;
typedef elem nil;

enum type { NIL_TYPE = 0, TEXT_TYPE = 1, HASH_TYPE = 2, LIST_TYPE = 3};

struct elem {
  enum type type;

  union {
    char   *text;
  } *data;

  size_t size;
  pair  *pairs;
};

struct pair {
  char   *key;
  int     index;
  struct elem   *elem;

  UT_hash_handle hh;
};

void elem_free(elem *e);
char * elem_to_s(elem *e);

nil * nil_init();
nil * nil_copy(nil *n);
void nil_free(nil *n);
char * nil_to_s(nil *n);

text * text_init(char *s);
text * text_copy(text *s);
void text_free(text *s);
char * text_to_s(text *s);

hash * hash_init();
hash * hash_copy(hash *h);
void hash_free(hash *h);
char * hash_to_s(hash *h);

void hash_insert(hash *h, char *key, elem *e);
void hash_remove(hash *h, char *key);
elem * hash_find(hash *h, char *key);
void hash_merge(hash *dst, hash *src);

list * list_init(size_t size);
list * list_copy(list *l);
void list_free(list *l);
char * list_to_s(list *l);

void list_insert(list *l, size_t index, elem *e);
void list_append(list *l, elem *e);
void list_remove(list *l, size_t index);
elem* list_find(list *l, size_t index);

#endif/*HASH_H*/

