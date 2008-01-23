/**
 * \file combin.h
 *
 * \brief Defines various combinatorial objects and algorithms.
 */
#ifndef COMBIN_H
#define COMBIN_H

extern "C" {
#include "wht.h"
}

#include <list>
#include <map>

using std::list;
using std::map;

typedef unsigned int uint;

/**
 * \brief A combinatic, e.g. a monotonically increasing sequence of k elements from Z_n.
 *
 * Example:
 *
 * All combinatics of size n = 5 (e.g elements of Z_5 = { 0, 1, 2, 3, 4} ) with length k = 2:
 \verbatim
  [0, 1]
  [0, 2]
  [0, 3]
  [0, 4]
  [1, 2]
  [1, 3]
  [1, 4]
  [2, 3]
  [2, 4]
  [3, 4]
 \endverbatim
 */
typedef list<uint> combin;

/**
 * \brief A composition of n, e.g. an ordered sequence of positive integers
 * that sum to n. 
 *
 * Example:
 *
 * All compositions of n = 4
 \verbatim
  4
  3 + 1
  1 + 3
  2 + 2
  2 + 1 + 1
  1 + 2 + 1
  1 + 1 + 2
  1 + 1 + 1 + 1
 \endverbatim
 */
typedef list<uint> compos;

typedef struct compos_node compos_node;

typedef list<struct compos_node *> compos_nodes;

/**
 * \brief A composition tree node.  
 *
 * Example:
 *
 * A composition tree of size n = 6
 *
  \dot
    digraph composition_tree {
        node [shape=circle, fontname=Helvetica, fontsize=10];
        edge [ arrowhead="node", style="solid" ];
        z [ label="6" ];
        y [ label="2" ];
        a [ label="4" ];
        b [ label="2" ];
        c [ label="2" ];
        d [ label="1" ];
        e [ label="1" ];
        z -> {y a};
        a -> {b c};
        c -> {d e};
    }
    \enddot
 *
 */
struct compos_node {
  uint value;               /**< Positive integer value */
  compos_nodes *children;   /**< Potential children     */
};

/**
 * \brief Computes n choose k in a stable manner, e.g. not prone to overflow.
 *
 * \param n     n, a positive integer
 * \param k     k, a positive integer
 * \return      n choose k, a positive integer
 */
ulong n_choose_k(ulong n, ulong k);


/**
 * \brief Generate a random combinatic of n with length k.
 *
 * \param n     Size
 * \param k     Length
 * \return      Combinatic
 */
combin * combin_k_rand(uint n, uint k);

/**
 * \brief Generate a random combinatic of n with length between a and b.
 *
 * This computation counts the number of possible combinatics of size k = a .. b 
 * and constructs a
 * cumulative probability distributation.
 *
 * \param n     Size
 * \param a     Minimum length
 * \param b     Maximum length
 * \return      Combinatic
 */
combin * combin_rand(uint n, uint a, uint b);

/**
 * \brief Convert a combinatic to a composition.
 *
 * \param n     Size of composition
 * \param c     Combinatic
 * \return      Composition
 */
compos * combin_to_compos(uint n, combin *c);

/**
 * \brief Generate a random composition of n with total number of elements between a and b.
 *
 * \param n     Size
 * \param a     Minimum number of elements
 * \param b     Maximum number of elements
 * \return      Composition
 */
compos * compos_rand(uint n, uint a, uint b);

combin * combin_enum_init(uint n, uint k);
combin * combin_enum_next();


/**
 * \brief Generate a random composition tree of size n.
 *
 * \param n     Total size
 * \param a     Minimum number of elements
 * \param b     Maximum number of elements
 * \param min   Minimum element
 * \param max   Maximum element
 * \return      Composition tree
 *
 * \bug Minimum element parameter is not working
 */
compos_node * compos_tree_rand(uint n, uint a, uint b, uint min, uint max);

/**
 * \brief Generate a random rightmost composition tree of size n.
 *
 * \param n     Total size
 * \param a     Minimum number of elements
 * \param b     Maximum number of elements
 * \param min   Minimum element
 * \param max   Maximum element
 * \return      Composition tree
 *
 * \bug This computation is non deterministic and some times does not halt!
 * \bug Minimum element parameter is not working
 */
compos_node * compos_tree_rand_right(uint n, uint a, uint b, uint min, uint max);

void compos_tree_free(compos_node *node);

char* 
compos_tree_to_string(compos_node *cpn);

/**
 * \brief Convert a composition tree into a WHT plan.
 *
 * Relabel nodes with children as split.
 * Relabel nodes without children as smalls of size compos_node::n
 *
 * \param cpn   Composition tree
 * \return      WHT plan
 */
Wht *
compos_tree_to_wht(compos_node *cpn);

#endif/*COMBIN_H*/
