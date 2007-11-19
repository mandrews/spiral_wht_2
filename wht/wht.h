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
#include <stdarg.h>

/*
 * Constants
 */

#define MAX_CODELET_NAME_SIZE  (32) 
  /**< Maximum string size for codelet identifiers. */
#define MAX_SPLIT_NODES        (32)
  /**< Maximum number of children for split codelets. */

#define MAX_CODELET_PARAMS      (4)
  /**< Maximum number of parameters for all codelets. */

#define MAX_ATTRIBUTES          (4) 
  /**< Maximum number of attributes for a WHT plan node.  */

#define UNSET_ATTRIBUTE        (-1)

#define UNSET_PARAMETER        (-1)

#define MAX_MSG_LEN           (256)

/**
 * \typedef wht_value
 * \brief The data type of input vectors applied by the transform 
 */
/**
 * \def WHT_STABILITY_THRESHOLD
 * \brief The fast algorithm is considered stable if the max norm 
 * differs from direct computation by less than this quantity 
 */
#if     WHT_FLOAT == 1
typedef float wht_value;
#define WHT_STABILITY_THRESHOLD   (1e-4)
#define WHT_TYPE_STRING   ("float") 
#endif/*WHT_FLOAT*/

#if     WHT_DOUBLE == 1
typedef double wht_value;
#define WHT_STABILITY_THRESHOLD   (1e-10)
#define WHT_TYPE_STRING   ("double") 
#endif/*WHT_DOUBLE*/

enum attr_names { interleave_by = 0, vector_size = 1 };


/*
 * Data structures and interfaces (function pointer signatures)
 */

/* Forward declarations, so typedef'd struct can contain itself */
typedef struct Wht Wht;
typedef struct split_children split_children;


/**
 * \typedef codelet_apply_fp
 *
 * \brief Interface (function pointer signature) for applying codelets to input
 * vector
 *
 * void (*codelet_apply)(Wht *W, long S, wht_value *x);
 *
 * \param Wht     Pointer to wht plan 
 * \param S       Stride at which to access the input vector
 * \param U       Base stride 
 * \param x       Input vector
 */
typedef void (*codelet_apply_fp)(Wht *W, long S, size_t U, wht_value *x);

/**
 * \typedef codelet_transform_fp
 *
 * \brief Interface (function pointer signature) for applying code
 * transformations to codelets 
 *
 * void (*codelet_transform_fp)(Wht *W);
 *
 * \param Wht     Pointer to wht plan 
 */
typedef void (*codelet_transform_fp)(Wht *W);

/**
 *\todo Remove this typedef alias once codelet registry is created with this new type
 */
typedef codelet_apply_fp codelet;

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

  codelet_apply_fp apply;
    /**< Recursive method for applying transform to input vector */

  void (*free)  (Wht *W);
    /**< Recursive method for freeing memory allocated by codelet */ 

  codelet_transform_fp transform;
    /**< Recursive method for if plan is an accept string in the language L(WHT) */ 

  char * (*to_string) (Wht *W); 
    /**< Recursive method for translating a codelet into a string */ 

  Wht *parent;

  size_t left;
  size_t right;

  split_children * children; 
    /**< Pointer to split children, should not be allocated unless codelet is a split */

  char *name; /**< Identifier for codelet, i.e. 'small' or  'split' */

  int params[MAX_CODELET_PARAMS];

  int attr[MAX_ATTRIBUTES]; /**< Attributes associated with WHT */

  char *error_msg;
};

/**
 * \struct split_children
 *
 * \brief Stores children of split codelet, i.e smaller WHTs.
 */
struct split_children {
  int nn;                          /**< Number of children  */
  int ns[MAX_SPLIT_NODES];         /**< Sizes of children   */
  Wht *Ws[MAX_SPLIT_NODES];        /**< Smaller WHTs        */
};

/**
 * \struct codelet_transform_entry
 *
 * \brief Structure for registering new codelet transforms with the package.
 *
 * \see registry.h
 *
 * \param name    Identifier associated with transform
 * \param params  Number of parameters accepted by transform
 * \param call    Interface to transform codelet
 */
typedef struct {
  char    name[MAX_CODELET_NAME_SIZE];
  size_t  params;
  codelet_transform_fp call;
} codelet_transform_entry;

#define TRANSFORM_ENTRY_END { "", 0, NULL } 
  /**< Place this at the end of the transform_registry to halt iteration */

/**
 * \struct codelet_apply_entry
 *
 * \brief Structure for registering new computational (unrolled) codelets with the package.
 *
 * \see codelets/codelets_registry.h
 *
 * \param size    Size of WHT computed by codelet
 * \param name    Identifier associated with codelet
 * \param params  Parameters assocaited with codelet
 * \param n       Number of parameters assocaited by codelet
 * \param call    Interface to initialize codelet codelet
 */
typedef struct {
  size_t  size;
  char    name[MAX_CODELET_NAME_SIZE];
  int     params[MAX_CODELET_PARAMS];
  size_t  n;
  codelet_apply_fp call;
} codelet_apply_entry;

#define CODELET_APPLY_ENTRY_END { 0, "", { 0 }, 0, (codelet_apply_fp) NULL }


/**
 *\todo Remove this typedef alias once codelet registry is created with this new type
 */
typedef codelet_apply_entry codelet_entry;


/*
 * Functions
 */

/**
 * \fn void * i_malloc(size_t size)
 *
 * \brief Internal malloc routine used inside package.
 *
 * \param   size Size of memory segment to allocated
 * \return  Pointer to memory segment
 *
 * For now this function just ensures optimal memory alignment.
 */
void * i_malloc(size_t size);

/**
 * \fn void i_free(void *p);
 *
 * \brief Internal free routine used inside package.
 *
 * \param   p   Pointer to memory segment to be freed.
 *
 * For now this function just calls free.
 */
void i_free(void *p);

Wht * parse(char *s);

void info(void);

Wht * direct(size_t n);

wht_value max_norm(const wht_value *x, const wht_value *y, size_t N);

wht_value * random_vector(size_t n);

void codelet_transform(Wht *W, const char *name, int params[], size_t n);

/**
 * \fn Wht * null_init(char *name, size_t n, int params[], size_t np);
 *
 * \brief Initializes a null codelet
 *
 * \param n       Size of codelet
 * \param name    Identified associated with codelet
 * \return        Pointer to allocated codelet
 *
 * Null codelets allocate memory and methods in a safe way but do transform the
 * input vector.  They are used for convience.
 */
Wht * null_init(size_t n, char *name);


/**
 * \fn Wht * split_init(char *name, Wht *Ws[], size_t nn, int params[], size_t np);
 *
 * \brief Initializes a split codelet
 *
 * \param Ws      Array of children codelets
 * \param nn      Number of children
 * \return        Pointer to allocated codelet
 *
 * All new split codelets registered with the package should 'derive' from this
 * codelet, i.e. first initialize the codelet with init_split and then
 * proceed to customize the codelet.
 */
Wht * split_init(Wht *Ws[], size_t nn);

/**
 * \fn Wht * small_init(char *name, size_t n, int params[], size_t np);
 *
 * \brief Initializes a small codelet
 *
 * \param name    Unique identifier for codelet
 * \param n       Size of codelet
 * \param params  Parameters for codelet, typically stored as attributes
 * \param np      Number of elements in parameter array
 * \return        Pointer to allocated codelet
 *
 * All new small codelets registered with the package should 'derive' from this
 * codelet, i.e. first initialize the codelet with init_small and then
 * proceed to customize the codelet.
 */
Wht * small_init(size_t n);


void null_apply(Wht *W, long S, size_t D, wht_value *x);

void small_apply(Wht *W, long S, size_t D, wht_value *x);

/**
 * \fn void split_apply(Wht *W, long S, wht_value *x);
 *
 * \brief Recursively apply transform to input vector
 *
 * \param   W   Transform
 * \param   S   Stride to apply transform at
 * \param   D   Base stride
 * \param   x   Input vector
 *
 *  A \f$ {\bf WHT}_{2^n} \f$ can be split into \f$ k \f$ \f$ {\bf WHT} \f$ 's of smaller size
 *  (according to \f$ n = n_1 + n_2 + \cdots + n_t \f$):
 *
  \f$ 
   \prod_{i=1}^t ({\bf I}_{2^{n_1 + \cdots + n_{i-1}}} 
      \otimes {\bf WHT}_{2^{n_i}}
      \otimes {\bf I}_{2^{n_{i+1} + \cdots + n_t}}) 
  \f$
 *
 * The \f$ {\bf WHT}_{2^n} \f$ is performed by stepping through this product
 * from right to left.
 */
void split_apply(Wht *W, long S, size_t D, wht_value *x);

void null_transform(Wht *W);

void small_transform(Wht *W);

void split_transform(Wht *W);

char * null_to_string(Wht *W);

char * small_to_string(Wht *W);

char * split_to_string(Wht *W);

void null_free(Wht *W);

void small_free(Wht *W);

void split_free(Wht *W);

void error_msg_set(Wht *W, char *format, ...);

char * error_msg_get(Wht *W);

char * append_params_to_name(const char *ident, int params[], size_t n);

/**
 * \todo Move these macros to an external interface.
 */
#define wht_apply(W, x) ((W->apply)(W, 1, 0, x))
#define wht_free(W) ((W->free)(W))
#define wht_to_string(W) ((W->to_string)(W))
#define wht_parse(s) (parse(s))
#define wht_info() (info())
#define wht_direct(n) (direct(n))
#define wht_max_norm(x,y,n) (max_norm(x,y,n))
#define wht_random_vector(n) (random_vector(n))
#define wht_error_msg(W)    (error_msg_get(W))

#define wht_exit(format, args...)  \
  {\
    fprintf (stderr, "error, "); \
    fprintf (stderr, format , ## args); \
    fprintf (stderr, "\n"); \
    exit(-1); \
  }

/**
 * \todo Remove this alias once change has been made in codelet generator
 */
#define wht_error wht_exit

#endif/* WHT_H */

