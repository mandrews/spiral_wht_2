/* Fast Walsh-Hadamard Transform (.h-file)
  ========================================
  MP, from 26.08.99

  The package wht implements the Walsh-Hadamard transform
  of 2-power size N = 2^n (WHT_N). The files included in 
  the package are given in the README file.
*/


#ifndef WHT_H
#define WHT_H

#include "config.h"
#include "wht_vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_CODELET_NAME_SIZE  32
#define MAX_SPLIT_NODES        32
#define MAX_CODELET_PARAMS      2
#define MAX_ATTRIBUTES          2

#if     WHT_FLOAT==1
typedef float wht_value;
#define WHT_STABILITY_THRESHOLD 1e-4
#define WHT_TYPE_STRING "float"
#endif/*WHT_FLOAT*/

#if     WHT_DOUBLE==1
typedef double wht_value;
#define WHT_STABILITY_THRESHOLD 1e-10
#define WHT_TYPE_STRING "double"
#endif/*WHT_DOUBLE*/

enum attr_names { interleave_by = 0, vector_size = 1 };

/* data type for the wht */
typedef struct wht {
  int N,                                          /* signal length */
      n;                                          /*       N = 2^n */

  void (*apply) (struct wht *W, long S, wht_value *x);
  void (*free)  (struct wht *W);     
  void (*guard) (struct wht *W, size_t right);     

  char * (*to_string) (struct wht *W);

  union {
    struct {
      int nn;                                 /* number of factors  */
      int ns[MAX_SPLIT_NODES];                /* size of factors    */
      struct wht *Ws[MAX_SPLIT_NODES];        /* the smaller wht's  */
    } split;
  } priv;

  char *name;

  int attr[MAX_ATTRIBUTES];

} Wht;

void * wht_malloc(size_t length);
void wht_free(void *p);

int wht_is2power(long n);
wht_value wht_max_norm(const wht_value *x, const wht_value *y, size_t N);

wht_value * wht_random(size_t n);

Wht * wht_direct(int n);

#define wht_apply(wht, S, x) ((wht->apply)(wht, S, x))

#define wht_error(format, args...)  \
  {\
    fprintf (stderr, "error, "); \
    fprintf (stderr, format , ## args); \
    fprintf (stderr, "\n"); \
    exit(-1); \
  }

Wht * wht_parse(char *s);
void wht_info(void);

Wht * wht_init_codelet(int n, char *name);
void wht_free_codelet(Wht *W);

typedef void (*codelet)(Wht *W, long S, wht_value *x);

typedef struct {
  size_t  size;
  char    name[MAX_CODELET_NAME_SIZE];
  codelet call;
  size_t  n;
  int     params[MAX_CODELET_PARAMS];
} codelet_entry;


typedef Wht * (*split)(char *name, Wht *Ws[], size_t nn, int params[], size_t np);

typedef struct {
  char    name[MAX_CODELET_NAME_SIZE];
  size_t  params;
  split   call;
} split_entry;

split lookup_split(const char *name, size_t params);


typedef Wht * (*small)(char *name, size_t n, int params[], size_t np);

typedef struct {
  char    name[MAX_CODELET_NAME_SIZE];
  size_t  params;
  small   call;
} small_entry;

small lookup_small(const char *name, size_t params);

#endif/* WHT_H */

