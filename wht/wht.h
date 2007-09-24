/* Fast Walsh-Hadamard Transform (.h-file)
  ========================================
  MP, from 26.08.99

  The package wht implements the Walsh-Hadamard transform
  of 2-power size N = 2^n (WHT_N). The files included in 
  the package are given in the README file.
*/

/**
 * \file wht.h
 *
 * \brief Internal header for WHT Package.
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

#define MAX_CODELET_NAME_SIZE  (32) 
  /**< Maximum string size for codelet identifiers. */
#define MAX_SPLIT_NODES        (32)
  /**< Maximum number of children for split codelets. */

#define MAX_CODELET_PARAMS      (2)
  /**< Maximum number of parameters for all codelets. */

#define MAX_ATTRIBUTES          (2) 
  /**< Maximum number of attributes for a WHT plan node.  */

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

/* Forward declaration, so typedef'd struct can contain itself */
typedef struct Wht Wht;

/**
 * \struct split_children
 *
 * \brief Stores children of split codelet, i.e smaller WHTs.
 */
typedef struct {
  int nn;                          /**< Number of children  */
  int ns[MAX_SPLIT_NODES];         /**< Sizes of children   */
  Wht *Ws[MAX_SPLIT_NODES];        /**< Smaller WHTs        */
} split_children;

/**
 * \struct Wht
 *
 * \brief Tree-like data structure for storing WHT plan.
 *
 * \todo Wht should be in lower case to follow standard C idiom, a typedef
 * alias can be used for backwards compatability
 */
struct Wht {
  int N; /**< Input signal length */
  int n; /**< N = 2^n             */

  void (*apply) (Wht *W, long S, wht_value *x); 
    /**< Recursive method for applying transform to input vector */

  void (*free)  (Wht *W);
    /**< Recursive method for freeing memory allocated by plan */ 

  void (*guard) (Wht *W, size_t right);     
    /**< Recursive method for determining validity of a plan */ 

  char * (*to_string) (Wht *W); 
    /**< Recursive method for translating a plan into a string */ 

  split_children * split; /**< 
    Pointer to split children, should not be allocated unless plan is a split */

  char *name; /**< Identifier for codelet, i.e. 'small' or  'split' */

  int attr[MAX_ATTRIBUTES]; /**< Attributes associated with WHT */
};

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
  int     params[MAX_CODELET_PARAMS];
  size_t  n;
  codelet call;
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

