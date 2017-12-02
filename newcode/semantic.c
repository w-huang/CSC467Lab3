#include "semantic.h"
#include "symbol.h"
#include <stdio.h>
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

#define UNIFORM_T (1 << 5)

static int current_scope = -1;
static int error_count = 0;
vector<char const *> args;

char const* get_op_str(int op) {
	if (op == OR) return "OR";
	else if (op == AND) return "AND";
	else if (op == EQ) return "EQ";
	else if (op == NEQ) return "NEQ";
	else if (op == '<') return "LESS THAN";
	else if (op == LEQ) return "LEQ";
	else if (op == '>') return "GREATER THAN";
	else if (op == GEQ) return "GEQ";
	else if (op == '+') return "PLUS";
	else if (op == '-') return "SUB";
	else if (op == '*') return "MUL";
	else if (op == '/') return "DIV";
	else if (op == '^') return "EXP";
	else if (op == '!') return "NOT";
	else if (op == '-') return "MINUS";
	else if (op == '(') return "OPEN_RB";
	else if (op == '[') return "OPEN_SB";

	return "DIDN'T FIND THE OP";
}

/* Returns 1 if the type is a boolean type */
int is_bool_type(char const* type) {
	if ((strcmp(type,"bool") == 0)  ||
		(strcmp(type,"bvec2") == 0) ||
		(strcmp(type,"bvec3") == 0) ||
		(strcmp(type,"bvec4") == 0)) 
	{
		return 1;
	}
	return 0;
}

/* Returns 1 if the type is a vector type */
int is_vec_type(char const* type) {
	if ((strcmp(type,"int") == 0)   ||
		(strcmp(type,"float") == 0) ||
		(strcmp(type,"bool") == 0))
	{
		return 0;
	}
	return 1;
}

/* Returns the size of the vector */
/* If scalar, return 1 */
int get_size_of_type(char const* type) {
	if ((strcmp(type,"int") == 0) ||
	   (strcmp(type,"float") == 0) ||
	   (strcmp(type,"bool") == 0))
	{
		return 1;
	}
	else if ((strcmp(type,"ivec2") == 0) ||
	   	    (strcmp(type,"bvec2") == 0) ||
	        (strcmp(type,"vec2") == 0))
	{
		return 2;
	}
	else if ((strcmp(type,"ivec3") == 0) ||
	        (strcmp(type,"bvec3") == 0) ||
	        (strcmp(type,"vec3") == 0))
	{
		return 3;
	}
	else if ((strcmp(type,"ivec4") == 0) ||
	        (strcmp(type,"bvec4") == 0) ||
	        (strcmp(type,"vec4") == 0))
	{
		return 4;
	}
	return -1;
}

/* Return the base type of the vector */
int get_vector_base_type(char const* type) {
	if ((strcmp(type,"int") == 0) ||
	   (strcmp(type,"ivec2") == 0) ||
	   (strcmp(type,"ivec3") == 0) || 
	   (strcmp(type,"ivec4") == 0))
	{
	   return INT_T;
	}
	else if ((strcmp(type,"float") == 0) ||
	   (strcmp(type,"vec2") == 0) ||
	   (strcmp(type,"vec3") == 0) || 
	   (strcmp(type,"vec4") == 0))
	{
	   return FLOAT_T;
	}
	else if ((strcmp(type,"bool") == 0) ||
	   (strcmp(type,"bvec2") == 0) ||
	   (strcmp(type,"bvec3") == 0) || 
	   (strcmp(type,"bvec4") == 0))
	{
	   return BOOL_T;
	}
	return -1;
}

/* Check if the next node is a literal/uniform variable */
/* Return 0 otherwise */
int check_next_node(node* ast, int& vec) {
	int kind = ast->kind;  
	switch(kind) {
		case INT_NODE:
			return INT_T;
		case FLOAT_NODE:
			return FLOAT_T;
		case BOOL_NODE:
			return BOOL_T;
		case EXPRESSION_VAR_NODE:
			return check_next_node(ast->expression_var_node.variable,vec);
		case VAR_NODE:
			if ((strcmp(ast->variable.identifier,"gl_Light_Half")) || 
			   (strcmp(ast->variable.identifier,"gl_Light_Ambient")) || 
			   (strcmp(ast->variable.identifier,"gl_Material_Shininess")) || 
			   (strcmp(ast->variable.identifier,"env1")) || 
			   (strcmp(ast->variable.identifier,"env2")) || 
			   (strcmp(ast->variable.identifier,"env3"))) {
			   return UNIFORM_T;
			}
			break;
		case TYPE_NODE:
			vec = ast->type.vec;
			return ast->type.type_name;
		case CONSTRUCTOR_NODE:
			return check_next_node(ast->constructor.type,vec);
		default:
			break;
	}
	return 0;
}

/* Return the qualifier of identifier from symbol table */
int get_qual(node* ast) {
	int kind = ast->kind;  
	switch(kind) {
		case INT_NODE:
			return INT_T;
		case FLOAT_NODE:
			return FLOAT_T;
		case BOOL_NODE:
			return BOOL_T;
		case EXPRESSION_VAR_NODE:
			return get_qual(ast->expression_var_node.variable);
		case VAR_NODE:
			return get_qualifier_of_identifier(ast->variable.identifier);
		case INDEXING_NODE:
			return get_qualifier_of_identifier(ast->array_indexing.identifier);
		default:
			break;
	}
	return 0;
}

int semantic_check(node* ast) {
	semantic_check_recurse(ast);
	return error_count;
}

char const* semantic_check_recurse(node *ast) {
	if (!ast)
		return 0;

	int kind = ast->kind;  	
	switch(kind) {
		case SCOPE_NODE: {
			current_scope++;
			insert_empty_table();
			
			semantic_check_recurse(ast->scope.declarations);
			semantic_check_recurse(ast->scope.statements);
			
			pop_symbol_table();
			current_scope--;
			break; 
		}

		case DECLARATIONS_NODE: {
			semantic_check_recurse(ast->declarations.declarations);
			semantic_check_recurse(ast->declarations.declaration);
			break;
		}  

		case STATEMENTS_NODE: {
			semantic_check_recurse(ast->statements.statements);
			semantic_check_recurse(ast->statements.statement);
			break;
		}

		case DECLARATION_NODE: {
			char const* type_name = semantic_check_recurse(ast->declaration.type);

			/* Check redeclaration */
			if (is_variable_inside_scope(ast->declaration.identifier) == 1) {
				fprintf(errorFile, "Redeclaration of variable %s in the same scope\n", 
						ast->declaration.identifier);
				error_count++;
				return NULL;
			}
			insert_into_symbol_table(current_scope,ast->declaration.identifier,type_name, NOT_CONST_Q);
			break;
		}

		case ASSIGNMENT_DECLARATION_NODE: {			
			char const* lhs_type = semantic_check_recurse(ast->assignment_declaration.type);

			/* Check redeclaration */
			if (is_variable_inside_scope(ast->assignment_declaration.identifier) == 1) {
				fprintf(errorFile, "Redeclaration of variable %s in the same scope\n", 
						ast->assignment_declaration.identifier);
				error_count++;
				return NULL;
			}

			int rhs_qual = get_qual(ast->assignment_declaration.expression);
			if (rhs_qual == RESULT_Q) {
				fprintf(errorFile, "Can't read variable of type result\n");
				error_count++;
			}

			/* Check for type mismatch */
			char const* rhs_type = semantic_check_recurse(ast->assignment_declaration.expression);
			if (strcmp(lhs_type, rhs_type) != 0 && strcmp(rhs_type,"any") != 0) {
				fprintf(errorFile, "Variable %s has type mismatch: expected type = %s received type = %s\n",
						ast->assignment_declaration.identifier, lhs_type, rhs_type);
				error_count++;
			}
			insert_into_symbol_table(current_scope,ast->assignment_declaration.identifier,lhs_type, NOT_CONST_Q);
			break;
		}

		case CONST_ASSIGNMENT_NODE: {
			char const* lhs_type = semantic_check_recurse(ast->const_assignment.type);
			
			/* Check redeclaration */
			if (is_variable_inside_scope(ast->const_assignment.identifier) == 1) {
				fprintf(errorFile, "Redeclaration of variable %s in the same scope\n", 
						ast->const_assignment.identifier);	
				error_count++;	
			}

			/* Check that next node of rhs expression is a uniform variable / literal */
			int vec = 0;
			int next_node_type = check_next_node(ast->const_assignment.expression,vec);
			int rhs_qual = get_qual(ast->const_assignment.expression);

			if (rhs_qual == ATTRIBUTE_Q) {
				fprintf(errorFile, "Can't assign an attribute to a const\n");
				error_count++;
			}

			if (next_node_type == 0) {
				fprintf(errorFile, "Const assignment to a non const literal / uniform type\n");
				error_count++;
			}
			else if ((next_node_type == UNIFORM_T) && (strcmp(lhs_type,"vec4") != 0)) {
				fprintf(errorFile, "Const assignment type mismatch: expected \"vec4\" "
						"type for uniform variable, received type = %s\n", lhs_type);
				error_count++;
			}
			else if (next_node_type != UNIFORM_T) {
				char const* rhs_type = get_type(next_node_type,vec);
				if (strcmp(lhs_type,rhs_type) != 0) {
					fprintf(errorFile, "Const assignment type mismatch: expected type = %s "
							"received type = %s\n",lhs_type,rhs_type);
					error_count++;
				}
				rhs_type = semantic_check_recurse(ast->const_assignment.expression);
				if (strcmp(lhs_type,rhs_type) != 0) {
					fprintf(errorFile, "Const assignment type mismatch: expected type = %s "
							"received type = %s\n",lhs_type,rhs_type);
					error_count++;
				}
			}
			insert_into_symbol_table(current_scope,ast->const_assignment.identifier,lhs_type,CONST_Q);
			break;
		}

		case ASSIGNMENT_STATEMENT_NODE: {			
			char const* lhs_type = semantic_check_recurse(ast->assignment_statement.left);
			char const* rhs_type = semantic_check_recurse(ast->assignment_statement.right);
			
			int lhs_qual = get_qual(ast->assignment_statement.left);
			int rhs_qual = get_qual(ast->assignment_statement.right);

			if (lhs_qual == CONST_Q && rhs_qual != UNIFORM_Q) {
				fprintf(errorFile, "Assignment to const variable of type %s\n", lhs_type);
				error_count++;
			}
			
			if (lhs_qual == UNIFORM_Q) {
				fprintf(errorFile, "Can't write to a uniform variable\n");
				error_count++;
			}
			else if (lhs_qual == ATTRIBUTE_Q) {
				fprintf(errorFile, "Can't write to a attribute variable\n");
				error_count++;
			}

			if (rhs_qual == RESULT_Q) {
				fprintf(errorFile, "Can't read variable of type result\n");
				error_count++;
			}

			if (strcmp(lhs_type,rhs_type) != 0 && strcmp(lhs_type,"any") != 0 && strcmp(rhs_type,"any") != 0) {
				fprintf(errorFile, "Variable type mismatch: lhs type = %s rhs type = %s\n", lhs_type, rhs_type);
				error_count++;
			}
			break;
		}

		case IF_ELSE_STATEMENT_NODE: {
			char const* condition_type = semantic_check_recurse(ast->if_else_statement.condition);
			if (strcmp(condition_type,"bool") != 0 && strcmp(condition_type,"any") != 0) {
				fprintf(errorFile,"Condition type of an if else statement must be a bool: "
						"received type: %s \n", condition_type);
				error_count++;
			}
			semantic_check_recurse(ast->if_else_statement.then_expression);
			semantic_check_recurse(ast->if_else_statement.else_expression);
			break;
		}

		case IF_STATEMENT_NODE: {
			char const* condition_type = semantic_check_recurse(ast->if_statement.condition);
			if (strcmp(condition_type,"bool") != 0 && strcmp(condition_type,"any") != 0) {
				fprintf(errorFile,"Condition type of an if statement must be a bool: "
						"received type: %s \n", condition_type);
				error_count++;
			}
			semantic_check_recurse(ast->if_statement.then_expression);
			break;
		}

		case STATEMENT_SCOPE_NODE: {
			semantic_check_recurse(ast->statement_scope.scope);
			break;
		}

		case TYPE_NODE: {
			char const* type = get_type(ast->type.type_name,ast->type.vec);
			return type;
		}

		case CONSTRUCTOR_NODE: {
			char const* constructor_type = semantic_check_recurse(ast->constructor.type);
			char const* constructor_base_type = get_type(get_vector_base_type(constructor_type),0);

			args.clear();
			semantic_check_recurse(ast->constructor.args);
			int size = args.size();

			if (size == 0) {
				fprintf(errorFile,"Unitialized constructor \n");
				error_count++;
			}
			else if (size != get_size_of_type(constructor_type)) {
				fprintf(errorFile,"Constructor size mismatch: expected size = %d, received size = %d \n", 
					   get_size_of_type(constructor_type), size);
				error_count++;
			}
			for (size_t i = 0; i < args.size(); i++) {
				if (strcmp(constructor_base_type, args[i]) != 0){
					fprintf(errorFile,"Constructor type mismatch. expected type = %s , "
							"received type = %s for constructor element [%ld] \n", constructor_base_type, args[i], i);
					error_count++;
				}
			}
			return constructor_type;			
		}

		case FUNCTION_NODE: {
			args.clear();
			semantic_check_recurse(ast->function.args);
			int size = args.size();
			
			if (ast->function.function_name == 0) { // dp3
				if (size != 2) {
					fprintf(errorFile,"dp3 function takes only two arguments. Arguments can be one of " 
						   "(vec3,vec3), (vec4,vec4), (ivec3,ivec3), (ivec4,ivec4). received size = %d \n", size);
					error_count++;
					return "any";
				}
				else if (size == 2) {
					if 	(((strcmp(args[0],"vec3") == 0) && (strcmp(args[1],"vec3") == 0)) || 
						((strcmp(args[0],"vec4") == 0) && (strcmp(args[1],"vec4") == 0)) ||  
						((strcmp(args[0],"ivec3") == 0) && (strcmp(args[1],"ivec3") == 0)) ||
						((strcmp(args[0],"ivec4") == 0) && (strcmp(args[1],"ivec4") == 0))) {
						return get_type(get_vector_base_type(args[0]),0);
					}
					else {
						fprintf(errorFile,"dp3 argument mismatch. Arguments can be one of " 
							"(vec3,vec3), (vec4,vec4), (ivec3,ivec3), (ivec4,ivec4). "
							"received argument types: %s, %s \n", args[0], args[1]);
						error_count++;
						return get_type(get_vector_base_type(args[0]),0);
					}
				}
			}
			else if (ast->function.function_name == 1) { // lit
				if (size != 1) {
					fprintf(errorFile,"lit function takes only one argument of type vec4 \n");
					error_count++;
					return "any";
				}
				else if (size == 1) {
					if 	(strcmp(args[0],"vec4") == 0) {
						return "vec4";
					}
					else {
						fprintf(errorFile, "lit argument mismatch: expected argument type = vec4 "
								"received argument type = %s \n", args[0]);
						error_count++;
						return "any";
					}
				}
			}
			else if (ast->function.function_name == 2) { // rsq
				if (size != 1) {
					fprintf(errorFile, "rsq function takes only one argument of type float or int\n");
					error_count++;
					return "any";
				}
				else if (size == 1) {
					if ((strcmp(args[0],"float") == 0) || (strcmp(args[0],"int") == 0)) {
						return "float";
					}
					else {
						fprintf(errorFile, "rsq argument mismatch. expected argument type = float/int "
								"received argument type = %s\n", args[0]);
						error_count++;
						return "any";
					}
				}
			}
			return "any";
		}

		case UNARY_EXPRESSION_NODE: {
			char const* right_expr_type = semantic_check_recurse(ast->unary_expr.right);
			if (ast->unary_expr.op == '!') {
				if (is_bool_type(right_expr_type)) {
					return right_expr_type;
				}
				else {
					fprintf(errorFile, "The expression to the right of logical not (!) must have " 
							"boolean type. received type: %s \n", right_expr_type);
					error_count++;
					return "any";
				}
			}
			else if (ast->unary_expr.op == '-') {
				if (is_bool_type(right_expr_type)) {
					fprintf(errorFile, "The expression to the right of unary minus (-) must have " 
							"arithmetic type received type: %s \n", right_expr_type);
					error_count++;
					return "any";
				}
				return right_expr_type;
			}
			return "any";
		}

		case BINARY_EXPRESSION_NODE: {
			char const* left_expr_type = semantic_check_recurse(ast->binary_expr.left);
			char const* right_expr_type = semantic_check_recurse(ast->binary_expr.right);
			char const* left_expr_basic_type = get_type(get_vector_base_type(left_expr_type),0);
			char const* right_expr_basic_type = get_type(get_vector_base_type(right_expr_type),0);
			int left_expr_size = get_size_of_type(left_expr_type);
			int right_expr_size = get_size_of_type(right_expr_type);
			int op = ast->binary_expr.op;

			if ((strcmp(left_expr_type,"any") == 0) ||
				(strcmp(right_expr_type,"any") == 0)) 
			{
				return "any";
			}

			/* add/subtract */
			if (ast->binary_expr.op == '+' || ast->binary_expr.op == '-') {
				if (is_bool_type(right_expr_type) || is_bool_type(left_expr_type)) {
					fprintf(errorFile, "The expression to the left (received type = %s) and right "
							"(received type = %s) of the binary expression (op = %s) must have the same arithmetic type.\n"
							, left_expr_type, right_expr_type, get_op_str(op));
					error_count++;
					return "any";
				}
				if (strcmp(left_expr_type,right_expr_type) != 0) 
				{
					fprintf(errorFile, "Left (%s) and right (%s) expression types of the binary " 
							"expression (op = %s) don't match\n", 
							left_expr_type, right_expr_type, get_op_str(op));
					error_count++;
					return "any";
				}	
				return left_expr_type;
			}
			/* multiply */
			else if (ast->binary_expr.op == '*') {
				/* Check for arithmetic types */
				if (is_bool_type(right_expr_type) || is_bool_type(left_expr_type)) {
					fprintf(errorFile, "The expression to the left (received type = %s) and "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same arithmetic type \n", left_expr_type, right_expr_type, get_op_str(op));
					error_count++;
					return "any";
				}

				/* Check if lhs and rhs are the same arithmetic types */
				if (strcmp(left_expr_type, right_expr_type) == 0) {
					return left_expr_type;
				}

				/* Check if its a scalar/vector multiplication, then the base types are the same */
				if (left_expr_size == 1 && right_expr_size > 1) {
					if ((strcmp(left_expr_basic_type,right_expr_basic_type) != 0)) {
						fprintf(errorFile, "The expression to the left (received type = %s) and "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same base type type \n", left_expr_type, right_expr_type, get_op_str(op));
						error_count++;
						return "any";
					}
					return right_expr_type;
				}

				if (left_expr_size > 1 && right_expr_size == 1) {
					if ((strcmp(left_expr_basic_type,right_expr_basic_type) != 0)) {
						fprintf(errorFile, "The expression to the left (received type = %s) and "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same base type \n", left_expr_type, right_expr_type, get_op_str(op));
							error_count++;						
						return "any";
					}
					return left_expr_type;
				}
				fprintf(errorFile, "The expression to the left (received type = %s) and "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same base type type \n", left_expr_type, right_expr_type, get_op_str(op));
				error_count++;
				return "any"; 
			}
			/* division/exponentiation */
			else if (ast->binary_expr.op == '/' || ast->binary_expr.op == '^') {
				/* Check if lhs and rhs are the same arithmetic types */
				if ((!is_bool_type(right_expr_type)) && (!is_bool_type(left_expr_type)) && 
					strcmp(left_expr_type, right_expr_type) == 0) {
					return left_expr_type;
				}
				fprintf(errorFile, "The expression to the left (received type = %s) and  "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same scalar arithmetic type \n", left_expr_type, right_expr_type, get_op_str(op));
				error_count++;
				return "any";
			}
			/* and/or */
			else if (ast->binary_expr.op == AND || ast->binary_expr.op == OR) {
				/* Check if lhs and rhs are the same boolean types */
				if (is_bool_type(right_expr_type) && is_bool_type(left_expr_type) && 
					strcmp(left_expr_type, right_expr_type) == 0) {
					return "bool";
				}
				fprintf(errorFile, "The expression to the left (received type = %s) and  "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same boolean type \n", left_expr_type, right_expr_type, get_op_str(op));
				error_count++;
				return "any";
			}
			/* comparison */
			else if (ast->binary_expr.op == '<' || ast->binary_expr.op == LEQ ||
				     ast->binary_expr.op == '>' || ast->binary_expr.op == GEQ)
			{
				/* Check if lhs and rhs are the same arithmetic scalar types */
				if ((!is_bool_type(right_expr_type)) && (!is_bool_type(left_expr_type)) && 
				   (!is_vec_type(right_expr_type)) && (!is_vec_type(left_expr_type)) && 
				   strcmp(left_expr_type, right_expr_type) == 0) {
					return "bool";
				}
				error_count++;
				fprintf(errorFile, "The expression to the left (received type = %s) and  "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same arithmetic scalar type \n", left_expr_type, right_expr_type, get_op_str(op));				return "any";
			}
			else if (ast->binary_expr.op == EQ || ast->binary_expr.op == NEQ) {
				/* Check if lhs and rhs are the same boolean types */
				if ((!is_bool_type(right_expr_type)) && (!is_bool_type(left_expr_type)) && 
					strcmp(left_expr_type, right_expr_type) == 0) {
					return "bool";
				}
				error_count++;
				fprintf(errorFile, "The expression to the left (received type = %s) and  "
							"right (received type = %s) of binary expression (op = %s) "
							"must have the same arithmetic type \n", left_expr_type, right_expr_type, get_op_str(op));
				return "any";
			}
			break;
		}

		case BOOL_NODE: {
			return get_type(BOOL_T,0);
		}

		case INT_NODE: {
			return get_type(INT_T,0);
		}

		case FLOAT_NODE: {
			return get_type(FLOAT_T,0);
		}

		case PAREN_EXPRESSION_NODE: {
			return semantic_check_recurse(ast->paren_expression.expression);
		}

		case EXPRESSION_VAR_NODE: {
			return semantic_check_recurse(ast->expression_var_node.variable);
		}

		case VAR_NODE: {
			char const* var_type = find_variable_in_symbol_table(ast->variable.identifier);
			if (var_type == NULL) {
				fprintf(errorFile, "Variable %s is undeclared in the current "
					   "or enclosing scope \n", ast->variable.identifier);
				error_count++;
				insert_into_symbol_table(current_scope,ast->variable.identifier,"any",NOT_CONST_Q);
				return "any";
			}
			return var_type; 
		}

		case INDEXING_NODE: {
			char const* var_type = find_variable_in_symbol_table(ast->array_indexing.identifier);
			if (var_type == NULL) {
				fprintf(errorFile, "Variable %s is undeclared in this current "
						"or enclosing scope \n", ast->array_indexing.identifier);
				error_count++;
			}
			if ((strcmp(var_type,"int") == 0) ||
				(strcmp(var_type,"bool") == 0) || 
				(strcmp(var_type,"float") == 0))
			{
				fprintf(errorFile, "Can't index the scalar variable %s \n", ast->array_indexing.identifier);
				error_count++;
				return var_type;
			}
			if (ast->array_indexing.index >= get_size_of_type(var_type)) {
				fprintf(errorFile, "Index out of bounds. Size of %s is only %d and index being referred to is %d \n", ast->array_indexing.identifier, get_size_of_type(var_type), ast->array_indexing.index);
				error_count++;
			}
			return get_type(get_vector_base_type(var_type),0);
		}

		case ARGUMENTS_NODE: {
			char const* res = semantic_check_recurse(ast->arguments.args);
			if (res) 
				args.push_back(res);
			res = semantic_check_recurse(ast->arguments.expression);
			if (res) 
				args.push_back(res);
			break;
		}

		default:
			break;
	}
  return NULL;
}