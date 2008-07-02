/*
 * Copyright (c) 2007 Drexel University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/**
 * \file combin.h
 * \author Michael Andrews
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
 * \param max   Maximum element
 * \return      Composition tree
 */
compos_node * compos_tree_rand(uint n, uint a, uint b, uint max);

/**
 * \brief Generate a random binary composition tree of size n.  
 * Only the first level of the tree must contain a binary split.
 *
 * \param n     Total size
 * \param a     Minimum number of elements
 * \param b     Maximum number of elements
 * \param max   Maximum element
 * \return      Composition tree
 */
compos_node * compos_tree_rand_bin(uint n, uint a, uint b, uint max);

void compos_tree_free(compos_node *node);

/**
 * \brief Convert a composition tree into a WHT plan.
 *
 * Relabel nodes with children as split.
 * Relabel nodes without children as smalls of size compos_node::n
 *
 * \param cpn   Composition tree
 * \return      WHT plan
 */
Wht * compos_tree_to_wht(compos_node *cpn);

compos_node * wht_to_compos_tree(Wht *W);

void compos_tree_print(compos_node *root);

compos_node * compos_tree_traverse(compos_node *r, uint min, uint max);

#endif/*COMBIN_H*/
