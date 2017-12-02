#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "symbol.h"

stack<vector<symbol_table_entry>> symbol_table;
size_t curr_scope = -1;

char const* get_qualifier_str(int qual) {
	if (qual == 0) 
		return "CONST_Q";
	else if (qual == 1)
		return "NOT_CONST_Q";
	else if (qual == 2)
		return "UNIFORM_Q";
	else if (qual == 3)
		return "ATTRIBUTE_Q";
	else if (qual == 4)
		return "RESULT_Q";
	return "DIDN'T FIND A QUALIFIER";
}

void pop_symbol_table() {
	if (symbol_table.empty() == false)
		symbol_table.pop();
}

void insert_empty_table() {
	vector<symbol_table_entry> empty_table;
	symbol_table.push(empty_table);
}

void insert_into_symbol_table(int scope, char const* identifier, char const* type_name, qualifier qual) {
	symbol_table_entry entry;
	entry.type_name = type_name;
	entry.identifier = identifier;
	entry.qual = qual;
	symbol_table.top().push_back(entry);
}

int is_variable_inside_scope(char* identifier) {
	if ((strcmp(identifier,"gl_FragColor") == 0) ||
		(strcmp(identifier,"gl_FragDepth") == 0) || 
		(strcmp(identifier,"gl_FragCoord") == 0) || 
		(strcmp(identifier,"gl_TexCoord") == 0) || 
		(strcmp(identifier,"gl_Color") == 0) || 
		(strcmp(identifier,"gl_Secondary") == 0) || 
		(strcmp(identifier,"gl_FogFragCoord") == 0) || 
		(strcmp(identifier,"gl_Light_Half") == 0) || 
		(strcmp(identifier,"gl_Light_Ambient") == 0) || 
		(strcmp(identifier,"gl_Material_Shininess") == 0) || 
		(strcmp(identifier,"gl_FragDepth") == 0) || 
		(strcmp(identifier,"env1") == 0) || 
		(strcmp(identifier,"env2") == 0) || 
		(strcmp(identifier,"env3") == 0)) 
	{
		return 1;
	}

	for (size_t i = 0; i < symbol_table.top().size(); i++) {
		if (strcmp(identifier,symbol_table.top()[i].identifier) == 0) {
			return 1;
		}
	}
	return 0;
}

char const* find_variable_in_symbol_table(char const* identifier) {
	if ((strcmp(identifier,"gl_FragColor") == 0) ||
		(strcmp(identifier,"gl_FragDepth") == 0) || 
		(strcmp(identifier,"gl_FragCoord") == 0) ||
		(strcmp(identifier,"gl_TexCoord") == 0) || 
		(strcmp(identifier,"gl_Color") == 0) || 
		(strcmp(identifier,"gl_Secondary") == 0) || 
		(strcmp(identifier,"gl_FogFragCoord") == 0) ||
		(strcmp(identifier,"gl_Light_Half") == 0) || 
		(strcmp(identifier,"gl_Light_Ambient") == 0) || 
		(strcmp(identifier,"gl_Material_Shininess") == 0) || 
		(strcmp(identifier,"gl_FragDepth") == 0) || 
		(strcmp(identifier,"env1") == 0) || 
		(strcmp(identifier,"env2") == 0) || 
		(strcmp(identifier,"env3") == 0)) 
	{
		return "vec4"; /* uniform */
	}
	stack<vector<symbol_table_entry>> table = symbol_table;

	while (table.empty() == false) {
		vector<symbol_table_entry> curr_table = table.top();
		for (size_t i = 0; i < curr_table.size(); i++) {
			if (strcmp(identifier,curr_table[i].identifier) == 0) {
				return curr_table[i].type_name;
			}
		}
		table.pop();
	}
	return NULL;
}

int get_qualifier_of_identifier(char const* identifier) {
	if ((strcmp(identifier,"gl_FragColor") == 0) ||
		(strcmp(identifier,"gl_FragDepth") == 0) || 
		(strcmp(identifier,"gl_FragCoord") == 0))
	{
		return RESULT_Q; /* result */
	}
	if ((strcmp(identifier,"gl_TexCoord") == 0) || 
		(strcmp(identifier,"gl_Color") == 0) || 
		(strcmp(identifier,"gl_Secondary") == 0) || 
		(strcmp(identifier,"gl_FogFragCoord") == 0))
	{
		return ATTRIBUTE_Q; /* attribute */
	}
	if ((strcmp(identifier,"gl_Light_Half") == 0) || 
		(strcmp(identifier,"gl_Light_Ambient") == 0) || 
		(strcmp(identifier,"gl_Material_Shininess") == 0) || 
		(strcmp(identifier,"gl_FragDepth") == 0) || 
		(strcmp(identifier,"env1") == 0) || 
		(strcmp(identifier,"env2") == 0) || 
		(strcmp(identifier,"env3") == 0)) 
	{
		return UNIFORM_Q; /* uniform */
	}

	stack<vector<symbol_table_entry>> table = symbol_table;
	while (table.empty() == false) {
		vector<symbol_table_entry> curr_table = table.top();
		for (size_t i = 0; i < curr_table.size(); i++) {
			if (strcmp(identifier,curr_table[i].identifier) == 0) {
				return curr_table[i].qual;
			}
		}
		table.pop();
	}
	return -1;
}

void print_symbol_table() {
	stack<vector<symbol_table_entry>> temp_table = symbol_table;
	int j = 0;
	while (temp_table.empty() == false) {
		vector<symbol_table_entry> curr_table = temp_table.top();
		j++;
		for (size_t i = 0; i < curr_table.size(); i++) {
			symbol_table_entry entry = curr_table[i];
			printf("Scope: %d Type: %s identifier: %s Qualifier: %s \n", j, entry.type_name, entry.identifier, get_qualifier_str(entry.qual));
		}
		temp_table.pop();
	}
}
