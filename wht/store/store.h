#ifndef HASH_H
#define HASH_H

#include "uthash.h"

#include <stdbool.h>

typedef struct elem elem;
typedef struct pair pair;

typedef elem string;
typedef elem hash;
typedef elem list;
typedef elem null;

enum type { NULL_TYPE = 0, STRING_TYPE = 1, HASH_TYPE = 2, LIST_TYPE = 3};

struct elem {
  enum type type;

  union {
    char   *string;
  } *data;

  size_t size;
  pair  *pairs;
};

struct pair {
  char   *key;
  int     index;
  elem   *elem;

  UT_hash_handle hh;
};

void elem_free(elem *e);
char * elem_to_s(elem *e);

null * null_init();
null * null_copy(null *n);
void null_free(null *n);
char * null_to_s(null *n);

string * string_init(char *s);
string * string_copy(string *s);
void string_free(string *s);
char * string_to_s(string *s);

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

