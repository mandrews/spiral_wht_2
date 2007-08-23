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
#include <stdbool.h>

#define CODELET_CALL_MAX_SIZE 40

/* data type for the signal */
typedef double wht_value;

/* data type for the wht */
typedef struct wht {
  int N,                                          /* signal length */
      n;                                               /*  N = 2^n */
  int nILNumber;
  void (*apply)(struct wht *W, long S, long D, wht_value *x);
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

int is2power(long n);

wht_value * wht_random(size_t n);

Wht * wht_direct(int n);

#define wht_apply(wht, S, D, x) ((wht->apply)(wht, S, D, x))

#define wht_error(format, args...)  \
  {\
    fprintf (stderr, "error, "); \
    fprintf (stderr, format , ## args); \
    fprintf (stderr, "\n"); \
    exit(-1); \
  }

/*
 * Place new codelet interfaces HERE
 */
#define REGISTERED_CODELETS 4

Wht * wht_parse_split();
Wht * wht_parse_small();
Wht * wht_parse_vector();
Wht * wht_parse_interleave();

typedef Wht * (*parse_codelet) (void);

const static
parse_codelet codelets[] = { 
  &wht_parse_vector,
  &wht_parse_interleave,
  &wht_parse_small,
  &wht_parse_split,
};

int wht_read_int();
void wht_next_codelet();
int wht_is_codelet(char *f);
void wht_require(char c);
int wht_check(char c);
Wht * wht_parse_next();
Wht * wht_parse(char *s);

Wht * wht_init_codelet(int n);
void wht_free_codelet(Wht *W);

typedef void (*codelet)(Wht *W, long S, long D, wht_value *x);

typedef struct {
  char    name[CODELET_CALL_MAX_SIZE];
  codelet call;
} codelet_entry;

#endif/* WHT_H */
