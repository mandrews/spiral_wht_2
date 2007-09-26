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

/*
 * Constants
 */

#define MAX_CODELET_NAME_SIZE  (32) 
  /**< Maximum string size for codelet identifiers. */
#define MAX_SPLIT_NODES        (32)
  /**< Maximum number of children for split codelets. */

#define MAX_CODELET_PARAMS      (2)
  /**< Maximum number of parameters for all codelets. */

#define MAX_ATTRIBUTES          (2) 
  /**< Maximum number of attributes for a WHT plan node.  */

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
 * \param x       Input vector
 */
typedef void (*codelet_apply_fp)(Wht *W, long S, wht_value *x);

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

  void (*guard) (Wht *W, size_t right);     
    /**< Recursive method for determining validity of a plan */ 

  char * (*to_string) (Wht *W); 
    /**< Recursive method for translating a codelet into a string */ 

  split_children * children; 
    /**< Pointer to split children, should not be allocated unless codelet is a split */

  char *name; /**< Identifier for codelet, i.e. 'small' or  'split' */

  int attr[MAX_ATTRIBUTES]; /**< Attributes associated with WHT */
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
 * \typedef split_init_fp
 *
 * \brief Interface (function pointer signature) for initializing split codelets
 *
 * Wht * (*split_init_fp)(char *name, Wht *Ws[], size_t nn, int params[], size_t * np);
 *
 * \param name    unique identifier for codelet
 * \param Ws      array of children codelets
 * \param params  parameters for codelet, typically stored as attributes
 * \param np      number of elements in parameter array
 */
typedef Wht * (*split_init_fp)(char *name, Wht *Ws[], size_t nn, int params[], size_t np);


/**
 * \typedef small_init_fp
 *
 * \brief Interface (function pointer signature) for initializing small codelets
 *
 * Wht * (*small_init_fp)(char *name, size_t n, int params[], size_t np);
 *
 * \param name    unique identifier for codelet
 * \param n       size of codelet
 * \param params  parameters for codelet, typically stored as attributes
 * \param np      number of elements in parameter array
 */
typedef Wht * (*small_init_fp)(char *name, size_t n, int params[], size_t np);


/**
 * \struct split_init_entry
 *
 * \brief Structure for registering new split codelets with the package.
 *
 * \see registry.h
 *
 * \param name    Identifier associated with split codelet
 * \param params  Number of parameters accepted by split codelet
 * \param call    Interface to initialize split codelet
 */
typedef struct {
  char    name[MAX_CODELET_NAME_SIZE];
  size_t  params;
  split_init_fp call;
} split_init_entry;

/* This halts the iteration of the registry */
#define SPLIT_INIT_ENTRY_END { "", 0, NULL } 

/**
 * \struct small_init_entry
 *
 * \brief Structure for registering new small codelets with the package.
 *
 * \see registry.h
 *
 * \param name    Identifier associated with small codelet
 * \param params  Number of parameters accepted by small codelet
 * \param call    Interface to initialize small codelet
 */
typedef struct {
  char    name[MAX_CODELET_NAME_SIZE];
  size_t  params;
  small_init_fp call;
} small_init_entry;

/* This halts the iteration of the registry */
#define SMALL_INIT_ENTRY_END { "", 0, NULL } 

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


wht_value wht_max_norm(const wht_value *x, const wht_value *y, size_t N);

wht_value * wht_random(size_t n);

Wht * wht_direct(int n);

#define wht_error(format, args...)  \
  {\
    fprintf (stderr, "error, "); \
    fprintf (stderr, format , ## args); \
    fprintf (stderr, "\n"); \
    exit(-1); \
  }

Wht * wht_parse(char *s);
void wht_info(void);

Wht * codelet_init(int n, char *name);
void codelet_free(Wht *W);

split_init_fp lookup_split(const char *name, size_t params);

small_init_fp lookup_small(const char *name, size_t params);

/**
 * \todo Move this macro to an external interface.
 */
#define wht_apply(wht, x) ((wht->apply)(wht, 1, x))

#endif/* WHT_H */

