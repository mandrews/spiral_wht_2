#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "store.h"

const size_t INDENT = 2;

/* Internal forward declarations */
elem * elem_init_abstract();
elem * elem_copy_dispatch(elem *e);
void elem_free_abstract(elem *e);
void elem_free_dispatch(elem *e);

char * elem_to_s_dispatch(elem *e, size_t indent);
char * elem_to_s_abstract(elem *e, size_t indent, char* l_delim, char* r_delim);

pair * pair_init_abstract(char *key, int index, elem *e);
void pair_free_abstract(pair *p);
char * pair_to_s_indent(pair *p, size_t indent);
int pair_index_sort(pair *a, pair *b);

pair * hash_find_pair(hash *h, char *key);
char * hash_to_s_indent(hash *h, size_t indent);

pair * list_find_pair(list *l, size_t index);
char * list_to_s_indent(list *l, size_t indent);

#if 0
char * 
itoa(int i)
{
  const size_t n = sizeof(int) * 8; 
  /* NOTE: n should log[10](length of maximum integer)
   * This should work since log[2](x) > log[10](x) 
   */

  char *buf;
  buf = malloc(sizeof(char) * n);

  snprintf(buf, n, "%d", i);

  return buf;
}
#endif

elem * 
elem_init_abstract()
{
  elem *e;

  e = malloc(sizeof(*e));

  e->data  = malloc(sizeof(*e->data));
  e->pairs = NULL; /* uthash handles alloc on add */

  return e;
}

elem * 
elem_copy_dispatch(elem *e)
{
  switch (e->type) {
    case HASH_TYPE:
      return hash_copy(e);
    case LIST_TYPE:
      return list_copy(e);
    case TEXT_TYPE:
      return text_copy(e);
    case NIL_TYPE:
      return nil_init();
    default:
      assert(false);
  }
}

void 
elem_free_abstract(elem *e)
{
  pair *p, *q;

  if (e == NULL)
    return;

  if (e->data != NULL)
    free(e->data);

  if (e->pairs != NULL) {
    p = e->pairs;

    while (p != NULL) {
      q = p->hh.next;
      HASH_DEL(e->pairs, p);
      pair_free_abstract(p);
      p = q;
    }

    free(e->pairs);
  }

  free(e);
}

void
elem_free_dispatch(elem *e)
{
  if (e == NULL)
    return;

  switch (e->type) {
    case HASH_TYPE:
      hash_free(e);
      break;
    case LIST_TYPE:
      list_free(e);
      break;
    case TEXT_TYPE:
      text_free(e);
      break;
    case NIL_TYPE:
      nil_free(e);
      break;
    default:
      assert(false);
  }
}

void
elem_free(elem *e)
{
  elem_free_dispatch(e);
}

char *
elem_to_s_dispatch(elem *e, size_t indent) 
{
  switch (e->type) {
    case HASH_TYPE:
      return hash_to_s_indent(e, indent);
    case LIST_TYPE:
      return list_to_s_indent(e, indent);
    case TEXT_TYPE:
      return text_to_s(e);
    case NIL_TYPE:
      return nil_to_s(e);
    default:
      assert(false);
  }
}

char *
elem_to_s(elem *e)
{
  return elem_to_s_dispatch(e, 0);
}

char *
elem_to_s_abstract(elem *e, size_t indent, char* r_delim, char* l_delim)
{
  char *buf, *tmp, *tab;
  pair *p;
  size_t len, m, n, i;

  tab = malloc(sizeof(char) * (indent + 1));

  strcpy(tab, "");
  for (i = 0; i < indent; i++)
    strcat(tab, " ");

  len = 5 + 2*indent; /*\n{\n ... }\0*/

  buf = malloc(sizeof(char) * len);

  if (indent != 0)
    strcpy(buf, "\n");
  else
    strcpy(buf, "");

  strcat(buf, tab);
  strcat(buf, r_delim);
  strcat(buf, "\n");

  p = e->pairs;
  while (p != NULL) {
    tmp = pair_to_s_indent(p, indent + INDENT);
    n = strlen(tmp);
    m = 3; /*X,\n\0*/

    len += (m + n);

    buf = realloc(buf, len);

    strcat(buf, tmp);

    p = p->hh.next;

    if (p != NULL) /* Commas for all but last */
      strcat(buf, ",");

    strcat(buf, "\n");

    free(tmp);
  }

  strcat(buf, tab);
  strcat(buf, l_delim);

  free(tab);

  return buf;
}


pair * 
pair_init_abstract(char *key, int index, elem *e)
{
  pair *p;

  p = malloc(sizeof(*p));

  if (key != NULL) {
     p->key = strdup(key);
  } else {
     p->key = NULL;
  }

  p->index  = index;
  p->elem   = elem_copy_dispatch(e);
   
  return p;
}

void 
pair_free_abstract(pair *p)
{
  if (p == NULL)
    return;

  if (p->key != NULL)
    free(p->key);

  if (p->elem != NULL)
    elem_free_dispatch(p->elem);

  free(p);
}

char *
pair_to_s_indent(pair *p, size_t indent)
{
  char *buf, *tmp, *tab;
  size_t m, n, k, i;

  tab = malloc(sizeof(char) * (indent + 1));
  strcpy(tab, "");
  for (i = 0; i < indent; i++)
    strcat(tab, " ");

  tmp = elem_to_s_dispatch(p->elem, indent);

  if (p->key != NULL) {
    m = strlen(p->key) + strlen(tab);
    n = strlen(tmp);
    k = 8 /* "X" : Y\0*/;

    buf = malloc(sizeof(char) * (m+n+k));

    sprintf(buf, "%s\"%s\" : %s", tab, p->key, tmp);
  } else {
    m = strlen(tab);
    n = strlen(tmp);
    k = 1; /*X\0*/

    buf = malloc(sizeof(char) * (m+n+k));

    sprintf(buf, "%s%s", tab, tmp);
  }

  free(tab);
  free(tmp);

  return buf;
}

nil * 
nil_init()
{
  elem *e;

  e = elem_init_abstract();
  e->type = NIL_TYPE;

  return e;
}

nil *
nil_copy(nil *n)
{
  assert(n->type == NIL_TYPE);
  return nil_init();
}

void 
nil_free(nil *n)
{
  if (n == NULL)
    return;

  assert(n->type == NIL_TYPE);
  elem_free_abstract(n);
}

char *
nil_to_s(nil *n)
{
  assert(n->type == NIL_TYPE);
  return strdup("nil");
}

/*
 * T E X T
 */

text * 
text_init(char *s)
{
  elem *e;

  e = elem_init_abstract();
  e->data->text = strdup(s);
  e->type = TEXT_TYPE;

  return e;
}

text *
text_copy(text *s)
{
  assert(s->type == TEXT_TYPE);
  return text_init(s->data->text);
}

void 
text_free(text *s)
{
  if (s == NULL)
    return;

  assert(s->type == TEXT_TYPE);

  /* NOTE: Import that this free'd first */
  if (s->data->text != NULL)
    free(s->data->text);

  elem_free_abstract(s);
}

char *
text_to_s(text *s)
{
  assert(s->type == TEXT_TYPE);

  char *buf;
  size_t m, n;

  m = strlen(s->data->text);
  n = 3 /*"X"\0*/;
  buf = malloc(sizeof(char) * (m + n));

  strcpy(buf, "\"");

  if (s->data->text != NULL) 
    strcat(buf, s->data->text);

  strcat(buf, "\"");

  return buf;
}

hash * 
hash_init()
{
  elem *e;

  e = elem_init_abstract();
  e->type = HASH_TYPE;

  return e;
}

hash *
hash_copy(hash *a)
{
  assert(a->type == HASH_TYPE);

  hash *b;
  pair *p;
  b = hash_init();

  p = a->pairs;

  while (p != NULL) {
    hash_insert(b, p->key, p->elem);
    p = p->hh.next;
  }

  return b;
}

void 
hash_free(hash *h)
{
  if (h == NULL)
    return;

  assert(h->type == HASH_TYPE);
  elem_free_abstract(h);
}

char *
hash_to_s_indent(hash *h, size_t indent)
{
  assert(h->type == HASH_TYPE);
  return elem_to_s_abstract(h, indent, "{", "}");
}

char *
hash_to_s(hash *h)
{
  assert(h->type == HASH_TYPE);
  return hash_to_s_indent(h, 0);
}

void 
hash_insert(hash *h, char *key, elem *e)
{
  assert(h->type == HASH_TYPE);

  pair *p;
  p = pair_init_abstract(key, -1, e);

  hash_remove(h, key);
  HASH_ADD_KEYPTR(hh, h->pairs, key, strlen(key), p);
}

void 
hash_remove(hash *h, char *key)
{
  assert(h->type == HASH_TYPE);

  pair *p;
  p = hash_find_pair(h, key);

  if (p != NULL) {
    HASH_DEL(h->pairs, p);
    pair_free_abstract(p);
  }
}

pair * 
hash_find_pair(hash *h, char *key)
{
  assert(h->type == HASH_TYPE);

  pair *p;

  HASH_FIND_STR(h->pairs, key, p);

  return p;
}

elem * 
hash_find(hash *h, char *key)
{
  assert(h->type == HASH_TYPE);

  pair *p;

  p = hash_find_pair(h, key);

  if (p == NULL)
    return NULL;
  else
    return p->elem;
}

void 
hash_merge(hash *dst, hash *src)
{
  assert(src->type == HASH_TYPE);
  assert(dst->type == HASH_TYPE);

  pair *p;

  p = src->pairs;

  while (p != NULL) {
    hash_insert(dst, p->key, p->elem);
    p = p->hh.next;
  }
}

list * 
list_init(size_t size)
{
  nil *n;
  list *l;
  size_t i;

  n = nil_init();
  l = elem_init_abstract();
  l->type = LIST_TYPE;

  l->size = size;
  for (i = 0; i < size; i++)
    list_insert(l, i, n);

  nil_free(n);

  return l;
}

list *
list_copy(list *a)
{
  assert(LIST_TYPE == a->type);

  list *b;
  pair *p;

  b = list_init(a->size);

  p = a->pairs;

  while (p != NULL) {
    list_insert(b, p->index, p->elem);
    p = p->hh.next;
  }

  return b;
}

void 
list_free(list *l)
{
  if (l == NULL)
    return;

  assert(LIST_TYPE == l->type);
  elem_free_abstract(l);
}

char *
list_to_s_indent(list *l, size_t indent)
{
  assert(LIST_TYPE == l->type);
  HASH_SORT(l->pairs, pair_index_sort);
  return elem_to_s_abstract(l, indent, "[", "]");
}

char *
list_to_s(list *l)
{
  assert(LIST_TYPE == l->type);
  return list_to_s_indent(l, 0);
}

void 
list_insert(list *l, size_t index, elem *e)
{
  assert(LIST_TYPE == l->type);
  assert(index < l->size);

  pair *p;
  p = pair_init_abstract(NULL, (int) index, e);

  list_remove(l, index);
  HASH_ADD_INT(l->pairs, index, p);
}

void 
list_append(list *l, elem *e)
{
  assert(LIST_TYPE == l->type);
  size_t i;

  i = l->size;
  l->size++;
  list_insert(l, i, e);
}

void 
list_remove(list *l, size_t index)
{
  assert(LIST_TYPE == l->type);
  assert(index < l->size);

  pair *p;
  p = list_find_pair(l, index);

  if (p != NULL) {
    HASH_DEL(l->pairs, p);
    pair_free_abstract(p);
  }
}

pair * 
list_find_pair(list *l, size_t index)
{
  assert(LIST_TYPE == l->type);
  assert(index < l->size);

  pair *p;

  HASH_FIND_INT(l->pairs, &index, p);

  return p;
}

elem * 
list_find(list *l, size_t index)
{
  assert(LIST_TYPE == l->type);
  assert(index < l->size);

  pair *p;
  p = list_find_pair(l, index);

  if (p == NULL)
    return NULL;
  else
    return p->elem;
}

int 
pair_index_sort(pair *a, pair *b)
{
  return (a->index - b->index);
}

