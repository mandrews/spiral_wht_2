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

typedef list<uint> combin;
typedef list<uint> compos;

typedef map<uint, double> combin_pdf;

typedef struct compos_node compos_node;

typedef list<struct compos_node *> compos_nodes;

struct compos_node {
  uint value;
  compos_nodes *children;
};

ulong n_choose_k(ulong n, ulong k);

combin::iterator combin_elem_rand(combin * c, uint n);

combin * combin_k_rand(uint n, uint k);

/* Random combinination given length is between a and b */
combin * combin_rand(uint n, uint a, uint b);

/* Convert a combinination to a compososition */
compos * combin_to_compos(uint n, combin *c);

/* Random compososition given length is between a and b */
compos * compos_rand(uint n, uint a, uint b);

compos_node * compos_tree_rand(uint n, uint a, uint b, uint min, uint max);
compos_node * compos_tree_rand_right(uint n, uint a, uint b, uint min, uint max);

void compos_tree_free(compos_node *node);

char* 
compos_tree_to_string(compos_node *cpn);

Wht *
compos_tree_to_wht(compos_node *cpn);

#endif/*COMBIN_H*/
