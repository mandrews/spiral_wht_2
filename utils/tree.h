#ifndef TREE_H
#define TREE_H

#include "wht.h"

#define MAX_FORMULA_LEN 1024

typedef struct _node {
   int size;
   int parent_index;
   int num_children;
   struct _node *parent;
   struct _node **children;
} node;

node * node_init(int size, int num_children);

void node_free(node *node);

char* tree_to_formula(node* tree);

Wht * node_to_wht(node *n);

#endif
