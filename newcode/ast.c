#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

node *ast = NULL;

char const* get_function(int func_id) {
    if (func_id == 0) return "dp3";
    else if (func_id == 1) return "lit";
    else return "rsq";
}

char const* get_operation(int op) {
	if (op == AND)
		return "AND";
	else if (op == OR)
		return "OR";
	else if (op == EQ)
		return "EQ";
	else if (op == NEQ)
		return "NEQ";
	else if (op == '<')
		return "<";
	else if (op == LEQ)
		return "LEQ";
	else if (op == '>')
		return ">";
	else if (op == GEQ)
		return "GEQ";
	else if (op == '+')
		return "+";
	else if (op == '-')
		return "-";
	else if (op == '*')
		return "*";
	else if (op == '/')
		return "/";
	else if (op == '^')
		return "^";
	else if (op == '!')
		return "!";
	else if (op == UMINUS)
		return "UMINUS";
	else if (op == '(')
		return "(";
	else if (op == '[')
		return "[";

	return "DIDN'T FIND AN OPERATION";
}

char const* get_type(int type_id, int vec_id) {
	if (type_id == INT_T) {
		return "int";
	} 
	else if (type_id == BOOL_T) {
		return "bool";
	}
	else if (type_id == FLOAT_T) {
		return "float";
	} 
	else if (type_id == IVEC_T) {
		if (vec_id == 0)
			return "ivec1";
		else if (vec_id == 1)
			return "ivec2";
		else if (vec_id == 2)
			return "ivec3";
        else if (vec_id == 3)
            return "ivec4";
	} 
	else if (type_id == BVEC_T) {
		if (vec_id == 0)
			return "bvec1";
		else if (vec_id == 1)
			return "bvec2";
		else if (vec_id == 2)
			return "bvec3";
        else if (vec_id == 3)
            return "bvec4";
	} 
	else if (type_id == VEC_T) {
		if (vec_id == 0)
			return "vec1";
		else if (vec_id == 1)
			return "vec2";
		else if (vec_id == 2)
			return "vec3";
        else if (vec_id == 3)
            return "vec4";
	} 
	return "DIDN'T FIND A TYPE";
}

void tab(int level, int num_spaces) {
	for (int i = 0; i < level*num_spaces; i++) {
		printf(" ");
	}
}

void ast_print(node * ast, int level) {
    if (!ast) return;
    int kind = ast->kind;
    
    switch(kind) {
        case SCOPE_NODE: {
            printf("\n");
            tab(level,1);
            printf("( SCOPE ");
            ast_print(ast->scope.declarations,level+1);
            ast_print(ast->scope.statements,level+1);
            tab(level,1);
            printf(")\n");
            break;
        }
            
        case DECLARATIONS_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( DECLARATIONS ");
            ast_print(ast->declarations.declarations,level+1);
            ast_print(ast->declarations.declaration,level+1);
            tab(level,1);
            printf( ")\n");
            break;
         }
            
        case STATEMENTS_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( STATEMENTS ");
            ast_print(ast->statements.statements,level+1);
            ast_print(ast->statements.statement,level+1);
            tab(level,1);
            printf( ")\n");
            break;
        }

        case DECLARATION_NODE:{
        	printf("\n");
            tab(level,1);
            printf("( DECLARATION %s ", ast->declaration.identifier);
            ast_print(ast->declaration.type,level+1);
            printf(")\n");
            break;
        }

        case ASSIGNMENT_DECLARATION_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( DECLARATION %s ", ast->assignment_declaration.identifier);
            ast_print(ast->assignment_declaration.type,level+1);
            ast_print(ast->assignment_declaration.expression,level+1);
            printf(")\n");
            break;
        }

        case CONST_ASSIGNMENT_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( CONST DECLARATION %s ", ast->const_assignment.identifier);
            ast_print(ast->const_assignment.type,level+1);
            ast_print(ast->const_assignment.expression,level+1);
            printf( ")\n");
            break;
        }

        case ASSIGNMENT_STATEMENT_NODE: {
        	printf("\n");
            tab(level,1);
        	printf("( ASSIGN ");
            ast_print(ast->assignment_statement.left,level+1);
            ast_print(ast->assignment_statement.right,level+1);
            printf( ")\n");
            break;
        }

        case IF_ELSE_STATEMENT_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( IF ");
            ast_print(ast->if_else_statement.condition,level+1);
            ast_print(ast->if_else_statement.then_expression,level+1);
            ast_print(ast->if_else_statement.else_expression,level+1);
            tab(level,1);
            printf( ")\n");
            break;
        }

        case IF_STATEMENT_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( IF ");
            ast_print(ast->if_statement.condition,level+1);
            ast_print(ast->if_statement.then_expression,level+1);
            tab(level,1);
            printf( ")\n");
            break;
        }

        case STATEMENT_SCOPE_NODE: {
            ast_print(ast->statement_scope.scope,level+1);
            break;
        }

        case TYPE_NODE: {
        	char const* type = get_type(ast->type.type_name, ast->type.vec);
           	printf("%s ", type);
            break;
        }

        case CONSTRUCTOR_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( CALL ");
            ast_print(ast->constructor.type,level+1);
            ast_print(ast->constructor.args,level+1);
            printf(") ");
            break;
        }

        case FUNCTION_NODE: {
        	printf("\n");
            tab(level,1);
            printf("( CALL ");
            char const* function_name = get_function(ast->function.function_name);
            printf("%s ",function_name);
            ast_print(ast->function.args,level+1);
            printf(") ");
            break;
        }

        case UNARY_EXPRESSION_NODE: {
            char const* operation = get_operation(ast->unary_expr.op);
            printf("( UNARY %s ", operation);
            ast_print(ast->unary_expr.right,level+1);
            printf(") ");
            break;
        }
            
        case BINARY_EXPRESSION_NODE: {
            char const* binary_operation = get_operation(ast->binary_expr.op); //get_operation(ast->binary_expr.op);
            printf("( BINARY %s ", binary_operation);
            ast_print(ast->binary_expr.left,level+1);
            ast_print(ast->binary_expr.right,level+1);
            printf(") ");
            break;
        }

        case BOOL_NODE: {
            char const* bool_str;
            if (ast->bool_c.bool_num == 1)
                bool_str = "true";
            else
                bool_str = "false";
            printf("%s ",bool_str);
            break;
        }

        case INT_NODE: {
            printf("%d ",ast->int_c.int_num);
            break;
        }

        case FLOAT_NODE: {
            printf("%f ",ast->float_c.float_num);
            break;
        }

        case PAREN_EXPRESSION_NODE: {
            ast_print(ast->paren_expression.expression,level+1);
            break;
        }

        case EXPRESSION_VAR_NODE: {
        	ast_print(ast->expression_var_node.variable,level+1);
        	break;
        }

        case VAR_NODE: {
            printf("%s ",ast->variable.identifier);
            break;
        }

        case INDEXING_NODE: {
            printf("( INDEX int %s %d ) ", ast->array_indexing.identifier, ast->array_indexing.index);
            break;
        }

        case ARGUMENTS_NODE: {
            ast_print(ast->arguments.args,level+1);
            ast_print(ast->arguments.expression,level+1);
            break;
        }

        default:
            break;
    }
}

node *ast_allocate(node_kind kind, ...) {
    va_list args;
    node *ast = (node *) malloc(sizeof(node));
    memset(ast, 0, sizeof *ast);
    ast->kind = kind;
    va_start(args, kind);
    
    switch(kind) {
        case SCOPE_NODE:
            ast->scope.declarations = va_arg(args, node*);
            ast->scope.statements = va_arg(args, node*);
            break;
            
        case DECLARATIONS_NODE:
            ast->declarations.declarations = va_arg(args, node*);
            ast->declarations.declaration = va_arg(args, node*);
            break;
            
        case STATEMENTS_NODE:
            ast->statements.statements = va_arg(args, node*);
            ast->statements.statement = va_arg(args, node*);
            break;
            
        case DECLARATION_NODE:
            ast->declaration.type = va_arg(args, node*);
            ast->declaration.identifier = va_arg(args, char*);
            break;
            
        case ASSIGNMENT_DECLARATION_NODE:
            ast->assignment_declaration.type = va_arg(args, node*);
            ast->assignment_declaration.identifier = va_arg(args, char*);
            ast->assignment_declaration.expression = va_arg(args, node*);
            break;
            
        case CONST_ASSIGNMENT_NODE:
            ast->const_assignment.type = va_arg(args, node*);
            ast->const_assignment.identifier = va_arg(args, char*);
            ast->const_assignment.expression = va_arg(args, node*);
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
            ast->assignment_statement.left = va_arg(args, node*);
            ast->assignment_statement.right = va_arg(args, node*);
            break;
            
        case IF_ELSE_STATEMENT_NODE:
            ast->if_else_statement.condition = va_arg(args, node*);
            ast->if_else_statement.then_expression = va_arg(args, node*);
            ast->if_else_statement.else_expression = va_arg(args, node*);
            break;
            
        case IF_STATEMENT_NODE:
            ast->if_statement.condition = va_arg(args, node*);
            ast->if_statement.then_expression = va_arg(args, node*);
            break;
            
        case STATEMENT_SCOPE_NODE:
            ast->statement_scope.scope = va_arg(args, node*);
            break;
            
        case TYPE_NODE:
            ast->type.type_name = va_arg(args, int);
            ast->type.vec = va_arg(args,int);
            break;
            
        case CONSTRUCTOR_NODE:
            ast->constructor.type = va_arg(args, node*);
            ast->constructor.args = va_arg(args, node*);
            break;
            
        case FUNCTION_NODE:
            ast->function.function_name = va_arg(args, int);
            ast->function.args = va_arg(args, node*);
            break;
            
        case UNARY_EXPRESSION_NODE:
            ast->unary_expr.right = va_arg(args, node*);
            ast->unary_expr.op = va_arg(args, int);
            break;
            
        case BINARY_EXPRESSION_NODE:
            ast->binary_expr.left = va_arg(args, node*);
            ast->binary_expr.right = va_arg(args, node*);
            ast->binary_expr.op = va_arg(args, int);
            break;
            
        case BOOL_NODE:
            ast->bool_c.bool_num = va_arg(args, int);
            ast->type.type_name = BOOL_T;
            ast->type.vec = 0;
            break;
            
        case INT_NODE:
            ast->int_c.int_num = va_arg(args, int);
            ast->type.type_name = INT_T;
            ast->type.vec = 0;
            break;
            
        case FLOAT_NODE:
            ast->float_c.float_num = va_arg(args, double);
            ast->type.type_name = FLOAT_T;
            ast->type.vec = 0;
            break;
            
        case PAREN_EXPRESSION_NODE:
            ast->paren_expression.expression = va_arg(args, node*);
            break;

        case EXPRESSION_VAR_NODE:
        	ast->expression_var_node.variable = va_arg(args, node*);
        	break;
            
        case VAR_NODE:
            ast->variable.identifier = va_arg(args, char*);
            break;
            
        case INDEXING_NODE:
            ast->array_indexing.identifier = va_arg(args, char*);
            ast->array_indexing.index = va_arg(args, int);
            break;
            
        case ARGUMENTS_NODE:
            ast->arguments.args = va_arg(args, node*);
            ast->arguments.expression = va_arg(args, node*);
            break;
            
        default:
            break;
    }
    va_end(args);
    return ast;
}

void ast_free(node *ast) {
    if (!ast) return;
    int kind = ast->kind;
    
    switch(kind) {
        case SCOPE_NODE: {
            ast_free(ast->scope.declarations);
            ast_free(ast->scope.statements);
            free(ast);
            ast = NULL;
            break;
        }
            
        case DECLARATIONS_NODE: {
            ast_free(ast->declarations.declarations);
            ast_free(ast->declarations.declaration);
            free(ast);
            ast = NULL;
            break;
         }
            
        case STATEMENTS_NODE: {
            ast_free(ast->statements.statements);
            ast_free(ast->statements.statement);
            free(ast);
            ast = NULL;
            break;
        }

        case DECLARATION_NODE:{
            ast_free(ast->declaration.type);
            free(ast);
            ast = NULL;
            break;
        }

        case ASSIGNMENT_DECLARATION_NODE: {
            ast_free(ast->assignment_declaration.type);
            ast_free(ast->assignment_declaration.expression);
            free(ast);
            ast = NULL;
            break;
        }

        case CONST_ASSIGNMENT_NODE: {
            ast_free(ast->const_assignment.type);
            ast_free(ast->const_assignment.expression);
            free(ast);
            ast = NULL;
            break;
        }

        case ASSIGNMENT_STATEMENT_NODE: {
            ast_free(ast->assignment_statement.left);
            ast_free(ast->assignment_statement.right);
            free(ast);
            ast = NULL;
            break;
        }

        case IF_ELSE_STATEMENT_NODE: {
            ast_free(ast->if_else_statement.condition);
            ast_free(ast->if_else_statement.then_expression);
            ast_free(ast->if_else_statement.else_expression);
            free(ast);
            ast = NULL;
            break;
        }

        case IF_STATEMENT_NODE: {
            ast_free(ast->if_statement.condition);
            ast_free(ast->if_statement.then_expression);
            free(ast);
            ast = NULL;
            break;
        }

        case STATEMENT_SCOPE_NODE: {
            ast_free(ast->statement_scope.scope);
            free(ast);
            ast = NULL;
            break;
        }

        case TYPE_NODE: {
           	free(ast);
            ast = NULL;
            break;
        }

        case CONSTRUCTOR_NODE: {
            ast_free(ast->constructor.type);
            ast_free(ast->constructor.args);
            free(ast);
            ast = NULL;
            break;
        }

        case FUNCTION_NODE: {
            ast_free(ast->function.args);
            free(ast);
            ast = NULL;
            break;
        }

        case UNARY_EXPRESSION_NODE: {
            ast_free(ast->unary_expr.right);
            free(ast);
            ast = NULL;
            break;
        }
            
        case BINARY_EXPRESSION_NODE: {
            ast_free(ast->binary_expr.left);
            ast_free(ast->binary_expr.right);
            free(ast);
            ast = NULL;
            break;
        }

        case BOOL_NODE: {
            free(ast);
            ast = NULL;
            break;
        }

        case INT_NODE: {
            free(ast);
            ast = NULL;
            break;
        }

        case FLOAT_NODE: {
            free(ast);
            ast = NULL;
            break;
        }

        case PAREN_EXPRESSION_NODE: {
            ast_free(ast->paren_expression.expression);
            free(ast);
            ast = NULL;
            break;
        }

        case EXPRESSION_VAR_NODE: {
        	ast_free(ast->expression_var_node.variable);
        	free(ast);
            ast = NULL;
        	break;
        }

        case VAR_NODE: {
            free(ast);
            ast = NULL;
            break;
        }

        case INDEXING_NODE: {
            free(ast);
            ast = NULL;
            break;
        }

        case ARGUMENTS_NODE: {
            ast_free(ast->arguments.args);
            ast_free(ast->arguments.expression);
            free(ast);
            ast = NULL;
            break;
        }

        default:
            break;
    }    
}