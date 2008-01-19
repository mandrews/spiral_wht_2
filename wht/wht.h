/**
 * \file wht.h
 *
 * \brief Internal header for WHT Package.
 */

#ifndef WHT_H
#define WHT_H

#include "config.h"

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

#define MAX_RULE_NAME_SIZE      ( 32) 
  /**< Maximum string size for rule nameifiers. */
#define MAX_SPLIT_NODES          ( 32)
  /**< Maximum number of children for split nodes. */

#define MAX_RULE_PARAMS          (  4)
  /**< Maximum number of parameters for all rules. */

#define MAX_ATTRIBUTES           (  4) 
  /**< Maximum number of attributes for a node.  */

#define UNSET_ATTRIBUTE          ( -1)

#define UNSET_PARAMETER          ( -1)

#define MAX_MSG_LEN              (256)

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
#define WHT_STABILITY_THRESHOLD (   1e-4)
#define WHT_TYPE_STRING         ("float") 
#endif/*WHT_FLOAT*/

#if     WHT_DOUBLE == 1
typedef double wht_value;
#define WHT_STABILITY_THRESHOLD (   1e-10)
#define WHT_TYPE_STRING         ("double") 
#endif/*WHT_DOUBLE*/

/** \todo Move this to the registry */
enum attr_names { interleave_by = 0, vector_size = 1 };

/*
 * Data structures and interfaces (function pointer signatures)
 */

/* Forward declarations, so typedef'd struct can contain itself */
typedef struct Wht Wht;
typedef struct rule rule;
typedef struct split_children split_children;


/**
 * \brief Interface (function pointer signature) for applying codelets to input
 * vector
 *
 * \param Wht     Pointer to Wht plan 
 * \param S       Stride at which to access the input vector
 * \param U       Base stride 
 * \param x       Input vector
 */
typedef void (*apply_fp)(Wht *W, long S, size_t U, wht_value *x);

/** \todo Remove this typedef once codelet generators are update with
 *  new nomenclature
 */
typedef apply_fp codelet_apply_fp;


/**
 * \brief Interface (function pointer signature) for attaching rules to 
 * plan.
 *
 * \param Wht     Pointer to wht plan 
 */
typedef void (*rule_fp)(Wht *W);

/**
 * \brief Annotated tree data structure for storing WHT plan.
 *
 * \todo Wht should be in lower case to follow standard C idiom, a typedef
 * alias can be used for backwards compatability
 */
struct Wht {
  int N; /**< Input signal length */
  int n; /**< N = 2^n             */

  apply_fp apply;
    /**< Recursive method for applying transform to input vector */

  struct rule *rule;
    /**< Rule attached to this node in plan */

  Wht *parent;
    /**< Pointer to parent node */

  size_t left;
    /**< \f$L\f$ in \f$ {\bf I}_{L} \otimes {\bf W}_{N} \otimes {\bf I}_{R} \f$ */

  size_t right;
    /**< \f$R\f$ in \f$ {\bf I}_{L} \otimes {\bf W}_{N} \otimes {\bf I}_{R} \f$ */

  split_children * children; 
    /**< Pointer to split children, should not be allocated unless codelet is a split */

  char *to_string; 

  int attr[MAX_ATTRIBUTES]; /**< Attributes associated with WHT, set by attaching rule */

  char *error_msg;
    /**< Error message in case plan construction or attaching rule goes awry */
};

/**
 * \brief Stores children of split codelet, i.e smaller WHTs.
 */
struct split_children {
  int nn;                          /**< Number of children  */
  int ns[MAX_SPLIT_NODES];         /**< Sizes of children   */
  Wht *Ws[MAX_SPLIT_NODES];        /**< Smaller WHTs        */
};

/**
 * \brief Structure for registering new rules with the package.
 *
 * \param name   Identifier associated with rule
 * \param params  Number of parameters associated with rule
 * \param call    Rule attachment function
 */
struct rule {
  char    name[MAX_RULE_NAME_SIZE];
  size_t  n;
  int     params[MAX_RULE_PARAMS];
    /**< Parameters for attaching rule */
  bool    is_small;
  rule_fp call;
};

/*
 * Functions
 */

/**
 * \brief Internal malloc routine used inside package.
 *
 * \param   size Size of memory segment to allocated
 *
 * \return  Pointer to memory segment
 */
void * i_malloc(size_t size);

/**
 * \brief Internal free routine used inside package.
 *
 * \param   p   Pointer to memory segment to be freed.
 */
void i_free(void *p);

char * i_itoa(int i);

Wht * parse(char *s);

void info(void);

Wht * direct(size_t n);

wht_value max_norm(const wht_value *x, const wht_value *y, size_t N);

wht_value * random_vector(size_t n);

rule * rule_init();

void rule_free();

void rule_attach(Wht *W, const char *name, int params[], size_t n, bool is_small);

void rule_eval(Wht *W);

void rule_eval_from_string(Wht *W, char *rule);


/**
 * \brief Initializes a node 
 *
 * \param n       Size of codelet
 * \param name    Identified associated with codelet
 * \return        Pointer to allocated codelet
 *
 * Null codelets allocate memory and methods in a safe way but do transform the
 * input vector.  They are used for convience.
 */
Wht * node_init(size_t n, char *name);


/**
 * \brief Initializes a split node
 *
 * \param Ws      Array of children nodes
 * \param nn      Number of children
 *
 * \return        Pointer to allocated node
 */
Wht * split_init(Wht *Ws[], size_t nn);

/**
 * \brief Initializes a small node
 *
 * \param n       Size of transform
 *
 * \return        Pointer to allocated node
 */
Wht * small_init(size_t n);


void null_apply(Wht *W, long S, size_t D, wht_value *x);

/**
 * \brief Recursively apply transform to input vector
 *
 * \param   W   Transform
 * \param   S   Stride to apply transform at
 * \param   D   Base stride
 * \param   x   Input vector
 */
void split_apply(Wht *W, long S, size_t D, wht_value *x);

char * name_to_string(Wht *W);

char * node_to_string(Wht *W);

void node_free(Wht *W);

void error_msg_set(Wht *W, char *format, ...);

char * error_msg_get(Wht *W);

/**
 * \todo Move these macros to an external interface.
 */
#define wht_apply(W, x) ((W->apply)(W, 1, 0, x))
#define wht_free(W) (node_free(W))
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

#endif/* WHT_H */

