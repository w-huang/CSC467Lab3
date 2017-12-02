#include "codegen.h"
#include "symbol.h"
#include <stdio.h>
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "semantic.h"

int current_scope = 0;
int tempVar_count = 1;
int num_parameters = 0;
int condVarCount = 1;


int is_literal(int kind) {
	if (kind == INT_NODE || kind == FLOAT_NODE || kind == BOOL_NODE)
		return 1;
	return 0;
}

int is_expression(int kind) {
	if (kind == BINARY_EXPRESSION_NODE || kind == UNARY_EXPRESSION_NODE || 
		kind == PAREN_EXPRESSION_NODE || kind == FUNCTION_NODE)
		return 1;
	return 0;
}

int is_identifier(int kind) {
	if (kind == EXPRESSION_VAR_NODE) {
		return 1;
	}
	return 0;
}

void check_predef_and_print(char const* name, int index) {
	if      (strcmp(name, "gl_FragColor") == 0) 		 printf("result.color");
	else if (strcmp(name, "gl_FragDepth") == 0) 		 printf("result.depth");
	else if (strcmp(name, "gl_FragCoord") == 0) 		 printf("fragment.position");
	else if (strcmp(name, "gl_TexCoord") == 0) 			 printf("fragment.texcoord"); 
	else if (strcmp(name, "gl_Color") == 0) 			 printf("fragment.color");
	else if (strcmp(name, "gl_Secondary") == 0) 	 	 printf("fragment.color.secondary");
	else if (strcmp(name, "gl_FogFragCoord") == 0) 		 printf("fragment.fogcoord");
	else if (strcmp(name, "gl_Light_Half") == 0) 		 printf("state.light[0].half");
	else if (strcmp(name, "gl_Light_Ambient") == 0) 	 printf("state.lightmodel.ambient");
	else if (strcmp(name, "gl_Material_Shininess") == 0) printf("state.material.shininess"); 
	else if (strcmp(name, "env1") == 0) 				 printf("program.env[1]");
	else if (strcmp(name, "env2") == 0) 				 printf("program.env[2]");
	else if (strcmp(name, "env3") == 0) 				 printf("program.env[3]");
	else 												 printf("%s", name);

	if      (index == 0) printf(".x");
    else if (index == 1) printf(".y");
    else if (index == 2) printf(".z");
    else if (index == 3) printf(".w");
}

void gen_code_unary_op(node* ast, char const* instr) {
	int expression = is_expression(ast->unary_expr.right->kind);
	if (!expression) {
		printf("TEMP tmpVar%d;\n",tempVar_count);
		printf("%s tmpVar%d,", instr,tempVar_count);
		genCode(ast->unary_expr.right);
		printf(",-1.0;\n");
	}
	else {
		int right_index = genCode(ast->unary_expr.right);
		printf("TEMP tmpVar%d;\n",tempVar_count);
		printf("%s tmpVar%d,tmpVar%d,-1.0;\n", instr,tempVar_count, right_index-1);
	}
	tempVar_count++;
}

void gen_code_binary_exp(node* ast, char const* instr, char const* instr2, int flip, int flip2) {
	int left_expression = is_expression(ast->binary_expr.left->kind);
	int right_expression = is_expression(ast->binary_expr.right->kind);
	int left_index = 0;
	int right_index = 0;

	if (left_expression && right_expression) {
		left_index = genCode(ast->binary_expr.left);
		right_index = genCode(ast->binary_expr.right);
		
		printf("TEMP tmpVar%d;\n", tempVar_count);

		if (flip) printf("%s tmpVar%d,tmpVar%d,tmpVar%d;\n",instr,tempVar_count,right_index-1,left_index-1);
		else 	  printf("%s tmpVar%d,tmpVar%d,tmpVar%d;\n",instr,tempVar_count,left_index-1,right_index-1);

		if (instr2) {
			tempVar_count++;
			printf("TEMP tmpVar%d;\n",tempVar_count);
			if (flip2) printf("%s tmpVar%d,tmpVar%d,tmpVar%d;\n",instr2,tempVar_count,right_index-1,left_index-1);
			else 	   printf("%s tmpVar%d,tmpVar%d,tmpVar%d;\n",instr2,tempVar_count,left_index-1,right_index-1);
		}
	}
	else if (left_expression && !right_expression) {
		left_index = genCode(ast->binary_expr.left);
		printf("TEMP tmpVar%d;\n", tempVar_count);

		if (flip) {
			printf("%s tmpVar%d,",instr,tempVar_count);
			genCode(ast->binary_expr.right);
			printf(",tmpVar%d;\n", left_index-1);
		}
		else {
			printf("%s tmpVar%d,tmpVar%d,",instr,tempVar_count,left_index-1);
			genCode(ast->binary_expr.right);
			printf(";\n");
		}

		if (instr2) {
			tempVar_count++;
			printf("TEMP tmpVar%d;\n",tempVar_count);
			if (flip2) {
				printf("%s tmpVar%d,",instr2,tempVar_count);
				genCode(ast->binary_expr.right);
				printf(",tmpVar%d;\n", left_index-1);
			}
			else {
				printf("%s tmpVar%d,tmpVar%d,",instr2,tempVar_count,left_index-1);
				genCode(ast->binary_expr.right);
				printf(";\n");
			}
		}
	}
	else if (!left_expression && right_expression) {
		right_index = genCode(ast->binary_expr.right);
		printf("TEMP tmpVar%d;\n",tempVar_count);

		if (flip) {
			printf("%s tmpVar%d,tmpVar%d,",instr,tempVar_count,right_index-1);
			genCode(ast->binary_expr.left);
			printf(";\n");
		}
		else {
			printf("%s tmpVar%d,",instr,tempVar_count);
			genCode(ast->binary_expr.left);
			printf(",tmpVar%d;\n", right_index-1);
		}

		if (instr2) {
			tempVar_count++;
			printf("TEMP tmpVar%d;\n",tempVar_count);
			if (flip2) {
				printf("%s tmpVar%d,tmpVar%d,",instr2,tempVar_count,right_index-1);
				genCode(ast->binary_expr.left);
				printf(";\n");
			}
			else {
				printf("%s tmpVar%d,",instr2,tempVar_count);
				genCode(ast->binary_expr.left);
				printf(",tmpVar%d;\n", right_index-1);
			}
		}
	}
	else {
		printf("TEMP tmpVar%d;\n",tempVar_count);
		printf("%s tmpVar%d,",instr,tempVar_count);
		if (flip) {
			genCode(ast->binary_expr.right);
			printf(",");
			genCode(ast->binary_expr.left);
			printf(";\n");
		} 
		else {
			genCode(ast->binary_expr.left);
			printf(",");
			genCode(ast->binary_expr.right);
			printf(";\n");
		}

		if (instr2) {
			tempVar_count++;
			printf("TEMP tmpVar%d;\n",tempVar_count);
			printf("%s tmpVar%d,",instr2,tempVar_count);
			if (flip2) {
				genCode(ast->binary_expr.right);
				printf(",");
				genCode(ast->binary_expr.left);
				printf(";\n");
			} 
			else {
				genCode(ast->binary_expr.left);
				printf(",");
				genCode(ast->binary_expr.right);
				printf(";\n");
			}
		}
	}
	tempVar_count++;
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
            printf("TEMP %s;\n", ast->declaration.identifier);
            break;
            
        case ASSIGNMENT_DECLARATION_NODE:
            if (!is_expression(ast->assignment_declaration.expression->kind)) {
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
        	if (!is_expression(ast->const_assignment.expression->kind)) {
	          	printf("PARAM %s = ", ast->const_assignment.identifier);
          		genCode(ast->const_assignment.expression);
          		printf(";\n");
            }
            else {
            	int exp_index = genCode(ast->const_assignment.expression);
            	printf("PARAM %s = tempVar%d;\n", ast->const_assignment.identifier, exp_index-1);
            }
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
        	if (!is_expression(ast->assignment_statement.right->kind)) {
	          	printf("MOV ");
	          	genCode(ast->assignment_statement.left);
	          	printf(", ");
          		genCode(ast->assignment_statement.right);
          		printf(";\n");
            }
            else {
            	int exp_index = genCode(ast->assignment_statement.right);
            	printf("MOV ");
            	genCode(ast->assignment_statement.left);
            	printf(", tempVar%d;\n",exp_index-1);
            }
            break;
            
        case IF_ELSE_STATEMENT_NODE:{
			condVarCount++;
			
			printf("TEMP condVar%d;\n", condVarCount);
			
			int cond_index = genCode(ast->if_statement.condition);
			printf("MOV condVar%d, tempVar%d;\n", condVarCount, cond_index);


			int then_expr_index = genCode(ast->if_statement.then_expression);
			tempVar_count++;
			printf("CMP tempVar%d, condVar%d, tempVar%d, tempVar%d;\n", 
				tempVar_count, 
				condVarCount, 
				then_expr_index,
				tempVar_count);
            //genCode(ast->if_statement.condition);
			//genCode(ast->if_statement.then_expression);
			
			condVarCount--;
			return tempVar_count;
            break;
		}
		case IF_STATEMENT_NODE:{
			condVarCount++;
			
			printf("TEMP condVar%d;\n", condVarCount);
			
			int cond_index = genCode(ast->if_statement.condition);
			printf("MOV condVar%d, tempVar%d;\n", condVarCount, cond_index);


			int then_expr_index = genCode(ast->if_statement.then_expression);
			tempVar_count++;
			printf("CMP tempVar%d, condVar%d, tempVar%d, tempVar%d;\n", 
				tempVar_count, 
				condVarCount, 
				then_expr_index,
				tempVar_count);
            //genCode(ast->if_statement.condition);
			//genCode(ast->if_statement.then_expression);
			
			condVarCount--;
			return tempVar_count;
            break;
		}
        case STATEMENT_SCOPE_NODE:
            //genCode(ast->statement_scope.scope);
            break;
            
        case TYPE_NODE:
            break;
            
        case CONSTRUCTOR_NODE: {
            printf("{");
            int vec = 0;
            int type = check_next_node(ast->constructor.type,vec);
            num_parameters = get_size_of_type(get_type(type,vec));
            genCode(ast->constructor.args);
            printf("}");
            break;
        }
            
        case FUNCTION_NODE:
        	printf("TEMP tmpVar%d;\n", tempVar_count);
        	if (ast->function.function_name == 0) { /* dp3 */
        		printf("DP3 tmpVar%d,", tempVar_count);
        		num_parameters = 2;
        	}
        	else if (ast->function.function_name == 1) { /* lit */
        		printf("LIT tmpVar%d,", tempVar_count);
        		num_parameters = 1;
        	}
        	else if (ast->function.function_name == 2) { /* rsq */
        		printf("RSQ tmpVar%d,", tempVar_count);
        		num_parameters = 1;
        	}
        	tempVar_count++;
            genCode(ast->function.args);
            printf(";\n");
            return tempVar_count;
            
        case UNARY_EXPRESSION_NODE:
        	if (ast->unary_expr.op == '-') { /* UNARY MINUS */
        		gen_code_unary_op(ast,"MUL");
        		return tempVar_count;
        	}
        	else if (ast->unary_expr.op == '!') { /* NOT */
        		gen_code_unary_op(ast,"NOT");
        		return tempVar_count;
        	}
            return -1; /* shouldn't get here */
            
        case BINARY_EXPRESSION_NODE:
        	/* Implemented as: x+y -(x*y) */
        	if (ast->binary_expr.op == OR) {
        		gen_code_binary_exp(ast,"ADD","MUL",false,false);
        		printf("SUB tmpVar%d,tmpVar%d,tmpVar%d;\n",tempVar_count,tempVar_count-2,tempVar_count-1);
        		tempVar_count++;
        		return tempVar_count;        	
        	}
        	else if (ast->binary_expr.op == '+' || ast->binary_expr.op == '-' ||
        			 ast->binary_expr.op == '*' || ast->binary_expr.op == '^' ||
        			 ast->binary_expr.op == AND || ast->binary_expr.op == GEQ || 
        			 ast->binary_expr.op == '<')
        	{
        		char const* instr;
				if (ast->binary_expr.op == '+')      instr = "ADD";
				else if (ast->binary_expr.op == '-') instr = "SUB";
				else if (ast->binary_expr.op == '*') instr = "MUL";
				else if (ast->binary_expr.op == '^') instr = "POW";
				else if (ast->binary_expr.op == AND) instr = "MUL";
				else if (ast->binary_expr.op == GEQ) instr = "SGE";
        		else if (ast->binary_expr.op == '<') instr = "SLT";

        		gen_code_binary_exp(ast,instr,NULL,false,false);
        		return tempVar_count;
        	}
        	/* Implemented as x * (1/y) */
        	else if (ast->binary_expr.op == '/') {
				int left_expression = is_expression(ast->binary_expr.left->kind);
				int right_expression = is_expression(ast->binary_expr.right->kind);
				int left_index = 0;
				int right_index = 0;

				if (left_expression) {
					left_index = genCode(ast->binary_expr.left);
				}
				
				if (right_expression) {
					right_index = genCode(ast->unary_expr.right);
        			printf("RCP tmpVar%d,tmpVar%d;\n", tempVar_count, right_index-1);
				}
				else {
					printf("RCP tmpVar%d,",tempVar_count);  
        			genCode(ast->binary_expr.right);
        			printf(";\n");
				}
				
				tempVar_count++;

				if (left_expression) {
        			printf("MUL tmpVar%d,tmpVar%d,tempVar%d;\n",tempVar_count,left_index-1,tempVar_count-1);
				}
				else {
					printf("MUL tmpVar%d,",tempVar_count);  
        			genCode(ast->binary_expr.left);
        			printf(",tmpVar%d;\n",tempVar_count-1);
				}
        		tempVar_count++;
        		return tempVar_count;
        	}
        	else if (ast->binary_expr.op == LEQ) {
        		gen_code_binary_exp(ast,"SUB",NULL,true,false);
        		printf("SGT tmpVar%d,tmpVar%d,{0.0,0.0,0.0,0.0}\n", tempVar_count,tempVar_count-1);
        		tempVar_count++;
        		return tempVar_count;
        	}
        	else if (ast->binary_expr.op == '>') {
        		gen_code_binary_exp(ast,"SUB",NULL,true,false);
        		printf("SLT tmpVar%d,tmpVar%d,{0.0,0.0,0.0,0.0}\n", tempVar_count,tempVar_count-1);
        		tempVar_count++;
        		return tempVar_count;
        	}
        	/* Implemented as x >= y && x <= y */
        	else if (ast->binary_expr.op == EQ) {
        		gen_code_binary_exp(ast,"SGE","SGE",false,true);
        		printf("MUL tmpVar%d,tmpVar%d,tmpVar%d\n", tempVar_count,tempVar_count-1,tempVar_count-2);
        		tempVar_count++;
        		return tempVar_count;
        	}
        	/* Implemented as !(x >= y && x <= y) */
        	else if (ast->binary_expr.op == NEQ) {
        		gen_code_binary_exp(ast,"SGE","SGE",false,true);
        		printf("MUL tmpVar%d,tmpVar%d,tmpVar%d\n", tempVar_count,tempVar_count-1,tempVar_count-2);
        		tempVar_count++;
        		printf("NOT tmpVar%d,tmpVar%d\n", tempVar_count,tempVar_count-1);
        		tempVar_count++;
        		return tempVar_count;
        	}
        	return -1; /* shouldn't get here */
            
        case BOOL_NODE:
			if (ast->bool_c.bool_num == 1) printf("1.0");  /* true */
			else 						   printf("-1.0"); /* false */	
        	
        	if (num_parameters > 1) {
        		num_parameters--;
        		printf(",");
        	}
            break;
            
        case INT_NODE:
        	printf("%d.0", ast->int_c.int_num); /* convert to floating point */
        	if (num_parameters > 1) {
        		num_parameters--;
        		printf(",");
        	}
        	break;
            
        case FLOAT_NODE:
        	printf("%f", ast->float_c.float_num);
			if (num_parameters > 1) {
        		num_parameters--;
        		printf(",");
        	}
        	break;
            
        case PAREN_EXPRESSION_NODE:
            genCode(ast->paren_expression.expression);
        	break;

        case EXPRESSION_VAR_NODE:
        	return genCode(ast->expression_var_node.variable);
            
        case VAR_NODE:
        	check_predef_and_print(ast->variable.identifier,-1);
			if (num_parameters > 1) {
        		num_parameters--;
        		printf(",");
        	}
            break; 
            
        case INDEXING_NODE: 
            check_predef_and_print(ast->array_indexing.identifier,ast->array_indexing.index);
            break; 

        case ARGUMENTS_NODE: {
            genCode(ast->arguments.args);  
            genCode(ast->arguments.expression);
			break;
		}
        default:
            return -1; /* shouldn't get here */
    }
    return 0;
}