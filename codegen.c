#include "codegen.h"
#include "symbol.h"
#include <stdio.h>
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

int current_scope = 0;
int tempVar_count = 0;

int is_literal(int kind) {
	if (kind == INT_NODE || kind == FLOAT_NODE || kind == BOOL_NODE)
		return 1;
	return 0;
}

int is_expression(int kind) {
	if (kind == BINARY_EXPRESSION_NODE || kind == UNARY_EXPRESSION_NODE)
		return 1;
	return 0;
}

int is_identifier(int kind) {
	if (kind == EXPRESSION_VAR_NODE) {
		return 1;
	}
	return 0;
}

int genCode(node* ast) {
	if (!ast)
		return 0;
	int kind = ast->kind;  	
    
    switch(kind) {
        case SCOPE_NODE:
        	current_scope++;
            genCode(ast->scope.declarations);
            genCode(ast->scope.statements);
            current_scope--;
            break;
            
        case DECLARATIONS_NODE:
            genCode(ast->declarations.declarations);
            genCode(ast->declarations.declaration);
            break;
            
        case STATEMENTS_NODE:
            genCode(ast->statements.statements);
            genCode(ast->statements.statement);
            break;
            
        case DECLARATION_NODE:
            printf("TEMP %s\n;", ast->declaration.identifier);
            break;
            
        case ASSIGNMENT_DECLARATION_NODE:
            if (is_literal(ast->assignment_declaration.expression->kind)) {
            	printf("TEMP %s;\n", ast->assignment_declaration.identifier);
	          	printf("MOV %s, ", ast->assignment_declaration.identifier);
          		genCode(ast->assignment_declaration.expression);
          		printf(";\n");
            }
            else {
            	int exp_index = genCode(ast->assignment_declaration.expression);
            	printf("TEMP %s;\n", ast->assignment_declaration.identifier);
            	printf("MOV %s,tempVar%d;\n", ast->assignment_declaration.identifier, exp_index-1);
            }
            break;
            
        case CONST_ASSIGNMENT_NODE:
            //genCode(ast->const_assignment.type);
            //genCode(ast->const_assignment.identifier);
            //genCode(ast->const_assignment.expression);
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
            //genCode(ast->assignment_statement.left);
            //genCode(ast->assignment_statement.right);
            break;
            
        case IF_ELSE_STATEMENT_NODE:
            //genCode(ast->if_else_statement.condition);
            //genCode(ast->if_else_statement.then_expression);
            //genCode(ast->if_else_statement.else_expression);
            break;
            
        case IF_STATEMENT_NODE:
            //genCode(ast->if_statement.condition);
            //genCode(ast->if_statement.then_expression);
            break;
            
        case STATEMENT_SCOPE_NODE:
            //genCode(ast->statement_scope.scope);
            break;
            
        case TYPE_NODE:
            //genCode(ast->type.type_name);
            //genCode(ast->type.vec);
            break;
            
        case CONSTRUCTOR_NODE:
            //genCode(ast->constructor.type);
            //genCode(ast->constructor.args);
            break;
            
        case FUNCTION_NODE:
            //genCode(ast->function.function_name);
            //genCode(ast->function.args);
            break;
            
        case UNARY_EXPRESSION_NODE:
        	if (ast->unary_expr.op == '-') { // UNARY MINUS
        		int literal = is_literal(ast->unary_expr.right->kind);
        		if (literal) {
        			printf("MUL tmpVar%d,", tempVar_count);
        			genCode(ast->unary_expr.right);
        			printf(",-1.0;\n");
        		}
        		else {
        			int right_index = genCode(ast->unary_expr.right);
        			printf("MUL tmpVar%d,tmpVar%d,-1.0;\n", tempVar_count, right_index-1);
        		}
        		tempVar_count++;
        		return tempVar_count;
        	}
        	else if (ast->unary_expr.op == '!') { // NOT
        		int literal = is_literal(ast->unary_expr.right->kind);
        		if (!literal) {
        			printf("NOT tmpVar%d,", tempVar_count);
        			genCode(ast->unary_expr.right);
        			printf(";\n");
        		}
        		else {
        			int right_index = genCode(ast->unary_expr.right);
        			printf("NOT tmpVar%d,tmpVar%d;\n", tempVar_count, right_index-1);
        		}
        		tempVar_count++;

        		return tempVar_count;
        	}
            return -1; /* shouldn't get here */
            
        case BINARY_EXPRESSION_NODE:
        	/* a && b = a * b */
        	if (ast->binary_expr.op == AND) {
        		
        		int left_literal = is_literal(ast->binary_expr.left->kind);
        		int right_literal = is_literal(ast->binary_expr.right->kind);
        		int left_count = 0;
        		int right_count = 0;

        		if (left_literal) {
        			printf("TEMP tmpVar%d,",tempVar_count);
        			left_count = tempVar_count++;
        			genCode(ast->binary_expr.left);
        			printf(";\n");
        		}

        		if (right_literal) {
        			printf("TEMP tmpVar%d,",tempVar_count);
        			genCode(ast->binary_expr.right);
        			printf(";\n");
        			right_count = tempVar_count++;
        		}
        		
        		printf("MUL tmpVar%d,",tempVar_count);
        		
        		if (left_literal)
        			printf("tmpVar%d,", left_count);
        		else {
        			genCode(ast->binary_expr.left);
        		}

        		if (right_literal)
        			printf("tmpVar%d;\n", right_count);
        		else {
        			genCode(ast->binary_expr.right);
        			printf(";\n");
        		}

        		tempVar_count++;
        		return tempVar_count;
        	}
        	/* a || b = (a + b) - (a * b) */
        	else if (ast->binary_expr.op == OR) {
        		int left_literal = is_literal(ast->binary_expr.left->kind);
        		int right_literal = is_literal(ast->binary_expr.right->kind);
        		int left_count = 0;
        		int right_count = 0;

        		if (left_literal) {
        			printf("TEMP tmpVar%d,",tempVar_count);
        			left_count = tempVar_count++;
        			genCode(ast->binary_expr.left);
        			printf(";\n");
        		}

        		if (right_literal) {
        			printf("TEMP tmpVar%d,",tempVar_count);
        			genCode(ast->binary_expr.right);
        			printf(";\n");
        			right_count = tempVar_count++;
        		}
        		
        		printf("ADD tmpVar%d,",tempVar_count);
        		
        		if (left_literal)
        			printf("tmpVar%d,", left_count);
        		else {
        			genCode(ast->binary_expr.left);
        			printf(",");
        		}

        		if (right_literal)
        			printf("tmpVar%d;\n", right_count);
        		else {
        			genCode(ast->binary_expr.right);
        			printf(";\n");
        		}
        		int add_index = tempVar_count++;

        		printf("MUL tmpVar%d,",tempVar_count);
        		
        		if (left_literal)
        			printf("tmpVar%d,", left_count);
        		else {
        			genCode(ast->binary_expr.left);
        			printf(",");
        		}

        		if (right_literal)
        			printf("tmpVar%d;\n", right_count);
        		else {
        			genCode(ast->binary_expr.right);
        			printf(";\n");
        		}
        		int mul_index = tempVar_count++;
        		printf("SUB tmpVar%d,tmpVar%d,tmpVar%d;\n",tempVar_count,add_index,mul_index);
        		tempVar_count++;
        		return tempVar_count;        	
        	}
        	else if (ast->binary_expr.op == '+' || ast->binary_expr.op == '-' ||
        			 ast->binary_expr.op == '*' || ast->binary_expr.op == '/' ||
        			 ast->binary_expr.op == '^')
        	{
        		int left_literal = is_literal(ast->binary_expr.left->kind);
        		int right_literal = is_literal(ast->binary_expr.right->kind);
        		int left_identifier = is_identifier(ast->binary_expr.left->kind);
        		int right_identifier = is_identifier(ast->binary_expr.right->kind);

        		int left_index = 0;
        		int right_index = 0;
        		
        		char const* instr;

        		if (ast->binary_expr.op == '+')
        			instr = "ADD";
        		else if (ast->binary_expr.op == '-')
        			instr = "SUB";
        		else if (ast->binary_expr.op == '*')
        			instr = "MUL";
        		else if (ast->binary_expr.op == '^')
        			instr = "POW";

        		if (left_literal && right_literal) {
        			printf("%s tmpVar%d,",instr,tempVar_count);  
        			genCode(ast->binary_expr.left);
        			printf(",");
        			genCode(ast->binary_expr.right);
        			printf(";\n");
        		}
        		else if (!left_literal && right_literal) {
        			if (left_identifier) {
        				printf("%s tmpVar%d,",instr,tempVar_count);  
        				genCode(ast->binary_expr.left);
	        			printf(",");
	        			genCode(ast->binary_expr.right);
	        			printf(";\n");
        			}
        			else {
        				left_index = genCode(ast->binary_expr.left);
						printf("%s tmpVar%d,tmpVar%d",instr,tempVar_count, left_index-1);  
	        			printf(",");
	        			genCode(ast->binary_expr.right);
	        			printf(";\n"); 
        			}
        		}
        		else if (left_literal && !right_literal) {
        			if (right_identifier) {
        				printf("%s tmpVar%d,",instr,tempVar_count); 
        				genCode(ast->binary_expr.left);
	        			printf(",");
	        			genCode(ast->binary_expr.right);
	        			printf(";\n");
        			}
        			else {
        				right_index = genCode(ast->binary_expr.right);
						printf("%s tmpVar%d,",instr,tempVar_count);  
						genCode(ast->binary_expr.left);
	        			printf(",tmpVar%d;\n",right_index-1);
        			}
        		}
        		else {
        			if (left_identifier && right_identifier) {
        				printf("%s tmpVar%d,",instr,tempVar_count);  
        				genCode(ast->binary_expr.left);
	        			printf(",");
	        			genCode(ast->binary_expr.right);
	        			printf(";\n");
        			}
        			else if (left_identifier && !right_identifier) {
        				right_index = genCode(ast->binary_expr.right);
        				printf("%s tmpVar%d,",instr,tempVar_count);  
        				genCode(ast->binary_expr.left);
	        			printf(",tmpVar%d;\n",right_index-1);
	        			printf(";\n");
        			}
        			else if (!left_identifier && right_identifier) {
        				left_index = genCode(ast->binary_expr.left);
        				printf("%s tmpVar%d,tmpVar%d,",instr,tempVar_count, left_index-1);  
        				genCode(ast->binary_expr.right);
	        			printf(";\n");
        			}
        			else {
        				left_index = genCode(ast->binary_expr.left);
        				right_index = genCode(ast->binary_expr.right);
						printf("%s tmpVar%d,tmpVar%d,tmpVar%d;\n",instr,tempVar_count,left_index-1,right_index-1); 
        			}
        		}
        		tempVar_count++;
        		return tempVar_count;
        	}
            break;
            
        case BOOL_NODE:
			if (ast->bool_c.bool_num == 1) {
				printf("1.0");
			}
			else {
				printf("-1.0");
			}   	
            break;
            
        case INT_NODE:
        	printf("%d.0", ast->int_c.int_num);
            break;
            
        case FLOAT_NODE:
        	printf("%f", ast->float_c.float_num);
            break;
            
        case PAREN_EXPRESSION_NODE:
            //genCode(ast->paren_expression.expression);
            break;

        case EXPRESSION_VAR_NODE:
        	genCode(ast->expression_var_node.variable);
        	//printf("aaa%s", ast->expression_var_node.variable);
        	break;
            
        case VAR_NODE:
            printf("%s", ast->variable.identifier);
            //genCode(ast->variable.identifier);
            break;
            
        case INDEXING_NODE:
            //genCode(ast->array_indexing.identifier);
            //genCode(ast->array_indexing.index);
            break;
            
        case ARGUMENTS_NODE:
            //genCode(ast->arguments.args);
            //genCode(ast->arguments.expression);
            break;
            
        default:
            break;
    }
}