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

#define MAX_RULE_NAME_SIZE       ( 32) 
  /**< Maximum string size for rule names */
#define MAX_SPLIT_NODES          ( 32)
  /**< Maximum number of children for split nodes */

#define MAX_RULE_PARAMS          (  4)
  /**< Maximum number of parameters for all rules */

#define MAX_ATTRIBUTES           (  4) 
  /**< Maximum number of attributes for a node  */

#define UNSET_ATTRIBUTE          ( -1)

#define UNSET_PARAMETER          ( -1)

#define MAX_MSG_LEN              (256)
  /**< Maximum length for error messages  */

/**
 * \typedef wht_value
 * \brief The data type of input vectors applied by the transform.
 */
/**
 * \def WHT_STABILITY_THRESHOLD
 * \brief The fast algorithm is considered stable if the max norm 
 * differs from direct computation by less than this quantity.
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

/** 
 * \brief Possible attributes associated with a node after a rule has been applied 
 *
 * \todo Figure out howto allow attributes to be set outside this header. 
 */
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
 * vector.
 *
 * \param W       Pointer to WHT plan
 * \param S       Stride at which to access the input vector
 * \param U       Base stride
 * \param x       Input vector
 */
typedef void (*apply_fp)(Wht *W, long S, size_t U, wht_value *x);

/** \todo Remove this typedef once codelet generators are update with
 *  new nomenclature.
 */
typedef apply_fp codelet_apply_fp;


/**
 * \brief Interface (function pointer signature) for attaching rules to 
 * plan.
 *
 * \param W         Pointer to WHT plan
 */
typedef void (*rule_fp)(Wht *W);

/**
 * \brief Annotated tree data structure for storing WHT plan.
 *
 * \todo Wht should be in lower case to follow standard C idiom, a typedef
 * alias can be used for backwards compatability.
 */
struct Wht {
  int N; /**< Input signal length */
  int n; /**< N = 2^n             */

  apply_fp apply;
    /**< Recursive method for applying transform to input vector */

  struct rule *rule;
    /**< Rule attached to this node in the plan */

  Wht *parent;
    /**< Pointer to parent node */

  size_t left;
    /**< \L in \I_L_X_W_N_I_R */

  size_t right;
    /**< \R in \I_L_X_W_N_I_R */

  split_children * children; 
    /**< Pointer to child nodes, should not be allocated unless node is a split node */

  char *to_string; 
    /**< Recursive string representation of WHT plan */

  int attr[MAX_ATTRIBUTES]; 
    /**< Attributes associated with WHT, set by attaching rule */

  char *error_msg;
    /**< Error message in case plan construction or attaching rule goes awry */
};

/**
 * \brief Stores children of split codelet, i.e smaller WHTs.  \attention
 * Children are stored anti lexigraphically, e.g \b RIGHT to \b LEFT.  This is
 * due in part to the evaluation order when applying the transform.
 */
struct split_children {
  int nn;                          /**< Number of children  */
  int ns[MAX_SPLIT_NODES];         /**< Sizes of children   */
  Wht *Ws[MAX_SPLIT_NODES];        /**< Smaller WHTs        */
};

/**
 * \brief Structure for registering new rules with the package.
 */
struct rule {
  char    name[MAX_RULE_NAME_SIZE]; /**< Unique name of rule */
  size_t  n;                        /**< Number of rule parameters */
  int     params[MAX_RULE_PARAMS];  /**< Runtime parameterization of rule*/
  bool    is_small;                 /**< Is this a rule applied to a smal node or a split node ?*/
  rule_fp call;                     /**< Function to evaluate after rule attachment */
};




/*
 * Internal Functions
 */

/**
 * \brief Internal malloc routine used inside package.
 *
 * \param size    Size of memory segment to allocated
 * \return        Pointer to memory segment
 */
void * i_malloc(size_t size);

/**
 * \brief Internal free routine used inside package.
 *
 * \param p        Pointer to memory segment to be freed
 */
void i_free(void *p);

/**
 * \brief Internal integer to ASCII function.
 *
 * \param i        Integer
 * \return         Pointer to allocated ASCII
 */
char * i_itoa(int i);




/*
 * Core Functions (wrapped by external macro interface)
 */

/**
 * \brief Parse a string into a WHT plan.  
 *
 * If there were no syntax and sematic errors Wht::error_msg should be \c NULL.
 *
 * \param s         Input string representing plan
 * \return          Pointer to WHT plan 
 */
Wht * parse(char *s);

/**
 * \brief Print configuration and compilation information and exit.
 */
void info(void);

/**
 * \brief Initialize a WHT plan for direct computation of transform.
 *
 * \param n         Size of WHT transform
 * \return          Pointer to WHT plan 
 */
Wht * direct(size_t n);

/**
 * \brief Compute the max norm between two input vectors, e.g the maximum
 * distance.
 *
 * \param x         Vector
 * \param y         Vector
 * \param N         Length of vectors
 * \return          Max Norm
 */
wht_value max_norm(const wht_value *x, const wht_value *y, size_t N);

/**
 * \brief Generate a random input vector for the WHT.
 *
 * \param N         Size of transform / input vector
 * \return          Input vector
 */
wht_value * random_vector(size_t N);




/*
 * Plan Node Functions
 */

/**
 * \brief Initializes a node. 
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
 * \brief Recursively free a WHT plan.
 *
 * \param W       Pointer to WHT plan
 */
void node_free(Wht *W);

/**
 * \brief Recursively convert a WHT plan into a string representation.
 *
 * \param W       Pointer to WHT plan 
 * \return        String representation of WHT plan
 */
char * node_to_string(Wht *W);

/**
 * \brief Convert a WHT node name into a string representation.
 *
 * This is just the name of the rule with it's parameters and size appended in
 * a syntactically correct fashion.
 *
 * \param W       Pointer to WHT plan. 
 * \return        String representation of WHT name
 */
char * name_to_string(Wht *W);


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
 * \param n       Log of size of transform
 *
 * \return        Pointer to allocated node
 */
Wht * small_init(size_t n);


/**
 * \brief Empty application of transform, doesn't change the vector.
 *
 * \param   W   WHT plan.
 * \param   S   Stride to apply transform at.
 * \param   D   Base stride.
 * \param   x   Input vector.
 */
void null_apply(Wht *W, long S, size_t D, wht_value *x);

/**
 * \brief Recursively apply transform to the input vector.
 *
 * \param   W   WHT plan.
 * \param   S   Stride to apply transform at.
 * \param   D   Base stride.
 * \param   x   Input vector.
 */
void split_apply(Wht *W, long S, size_t D, wht_value *x);




/*
 * Rule Functions 
 */

/**
 * \brief Initializes a rule.
 *
 * \param name    Name associated with rule
 * \return        Pointer to allocated rule
 */
rule * rule_init(char *name);

/**
 * \brief Frees memory allocated by rule.
 *
 * \param p        Pointer to memory segment to be freed
 */
void rule_free(rule *p);

/**
 * \brief Attach a rule to a node in a WHT plan.  Rule remains invariant until
 * rule_eval is called at which time rule::call is evaulated with the node.
 *
 * \param W         Node to attach rule to
 * \param name      Name associated with rule
 * \param params    Runtime parameters for rule evaluation
 * \param n         Number of parameters 
 * \param is_small  Is the rule for small nodes or split nodes? 
 */
void rule_attach(Wht *W, const char *name, int params[], size_t n, bool is_small);

/**
 * \brief TODO
 * \param W         Root of WHT plan
 */
void rule_eval(Wht *W);

void rule_eval_from_string(Wht *W, char *rule);




/*
 * Misc Functions 
 */

void error_msg_set(Wht *W, char *format, ...);

char * error_msg_get(Wht *W);



/*
 * External Macro Interfaces
 */

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

