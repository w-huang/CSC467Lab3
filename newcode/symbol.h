#ifndef _SYMBOL_H
#define _SYMBOL_H
#include <vector>
#include <stack>

using namespace std;

typedef enum {
	CONST_Q = 1,
	NOT_CONST_Q = 2,
	UNIFORM_Q = 3,
	ATTRIBUTE_Q = 4,
	RESULT_Q = 5
} qualifier;

struct symbol_table_entry {
	char const* type_name;
	char const* identifier;
	qualifier qual;
};
 
void insert_into_symbol_table(int, char const*, char const*, qualifier);
void pop_symbol_table();
void print_symbol_table();
int is_variable_inside_scope(char*);
char const* find_variable_in_symbol_table(char const* identifier);
void insert_empty_table();
int get_qualifier_of_identifier(char const*);

#endif

