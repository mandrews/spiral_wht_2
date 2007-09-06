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

#define CODELET_CALL_MAX_SIZE 40
#define SPLIT_MAX_FACTORS 40

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

/* data type for the wht */
typedef struct wht {
  int N,                                          /* signal length */
      n;                                               /*  N = 2^n */
  int nILNumber;
  void (*apply)(struct wht *W, long S, wht_value *x);
  void (*free) (struct wht *W);     
  char * (*to_string) (void);
  union {
    struct {
      int nn;                                 /* number of factors */
      int ns[SPLIT_MAX_FACTORS];                /* size of factors */
      struct wht *Ws[SPLIT_MAX_FACTORS];      /* the smaller wht's */
    } split;
  } priv;
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

Wht * wht_init_codelet(int n);
void wht_free_codelet(Wht *W);

typedef void (*codelet)(Wht *W, long S, wht_value *x);

typedef struct {
  char    name[CODELET_CALL_MAX_SIZE];
  codelet call;
} codelet_entry;

#endif/* WHT_H */

