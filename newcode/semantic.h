#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "ast.h"
#include "symbol.h"


int semantic_check(node *);
char const* semantic_check_recurse(node *);
int check_next_node(node* ast, int& vec);
int get_size_of_type(char const*);

#endif