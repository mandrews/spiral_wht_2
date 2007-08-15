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

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#define WHT_NUMBER    1
#define WHT_CODELET   2

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

#define wht_apply(wht, S, D, x) ((wht->apply)(wht, S, D, x))

#define wht_error(format, args...)  \
  {\
    fprintf (stderr, "error, "); \
    fprintf (stderr, format , ## args); \
    fprintf (stderr, "\n"); \
    exit(-1); \
  }

/*
 * Place new codelet interfaces here
 */

#include "small.h"

Wht * parse_split();
#define REGISTERED_CODELETS 2

typedef Wht * (*parse_callback) (void);

static
parse_callback callbacks[REGISTERED_CODELETS] = { 
  &parse_small,
  &parse_split,
};

union {
  char ident[CODELET_NAME_MAX_SIZE];
  int  value;
} wht_token;

int wht_read_int();
void wht_next_codelet();
int wht_is_codelet(char *f);
void wht_require(char c);
int wht_check(char c);
Wht * wht_parse_helper();

#endif/* WHT_H */
