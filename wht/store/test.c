#include <stdio.h>
#include <stdlib.h>

#include "store.h"

#define FAIL() \
  { \
    fprintf(stderr, "assertion failed @ %s:%d\n",__FILE__,__LINE__); \
    exit(1); \
  }

#define ASSERT_STRING_EQUAL(A,B) \
  { \
    if (strcmp(A,B) != 0) \
      FAIL(); \
  }

#define ASSERT_NULL(A) \
  { \
    if (A != NULL) \
      FAIL(); \
  }

int 
main()
{
  hash *h1, *h2;
  list *l1;
  string *s1, *s2, *t3;
  char *buf;

  h1 = hash_init();
  h2 = hash_init();
  l1 = list_init(4);
  s1 = string_init("foo");
  s2 = string_init("bar");

  hash_insert(h1, "1A", s1);
  t3 = hash_find(h1, "1A");

  ASSERT_STRING_EQUAL(s1->data->string, t3->data->string);

  hash_remove(h1, "1A");
  t3 = hash_find(h1, "1A");

  ASSERT_NULL(t3);

  hash_insert(h1, "1A", s1);
  hash_insert(h1, "1A", s2);
  t3 = hash_find(h1, "1A");

  ASSERT_STRING_EQUAL(s2->data->string, t3->data->string);
  list_insert(l1, 0, s1);
  list_insert(l1, 1, s2);
  list_append(l1, s2);

  hash_insert(h1, "1B", l1);

  hash_insert(h2, "4C", s1);
  hash_insert(h1, "5D", h2);

  buf = hash_to_s(h1);
  printf("%s\n", buf);
  free(buf);

  hash_free(h1);
  hash_free(h2);
  list_free(l1);
  string_free(s1);
  string_free(s2);

  return 0;
}
