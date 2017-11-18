#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

//hello
node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
  va_list args;

  // make the node
  node *ast = (node *) malloc(sizeof(node));
  memset(ast, 0, sizeof *ast);
  ast->kind = kind;

  va_start(args, kind); 

  switch(kind) {
  
  case SCOPE_NODE:
    ast->declarations = va_arg(args, node*);
    ast->statements = va_arg(args,node*);
    break;
  case EXPRESSION_NODE:
    break;
  case UNARY_EXPRESION_NODE:
    ast->op = va_arg(args, int);
    ast->right = va_arg(args,node*);
    break;
  case BINARY_EXPRESSION_NODE:
    ast->binary_expr.op = va_arg(args, int);
    ast->binary_expr.left = va_arg(args, node *);
    ast->binary_expr.right = va_arg(args, node *);  
    break;
  case INT_NODE:
    ast->val = va_args(args,int);
    break;
  case FLOAT_NODE:
    ast->val = va_args(args,float);
    break;
  case VAR_NODE:
    ast->id = va_arg(args, char*);
    ast->is_array = va_arg(args, int);
    ast->array_index = va_arg(args, int);
    break;
  case FUNCTION_NODE:
    ast->function = va_arg(args, int);
    ast->arguments = va_arg(args, node*);
    break;
  case CONSTRUCTOR_NODE:
    ast->type = va_arg(args,node*);
    ast->arguments = va_arg(args, node*);
    break;
  case TYPE_NODE:
    ast->token_type = va_arg(args, int);
    ast->length = va_args(args, int);
    break;
  case BOOL_NODE:
    ast->value = va_args(args,int);
    break;
  case STATEMENT_NODE:
    ast->variable = va_args(args, node*);
    ast->expression = va_args(args, node*);
    break;
  case STATEMENTS_NODE:
    ast->statements = va_args(args,node*);
    ast->statement = va_args(args, node*);
    break;
  case ARGUMENTS_NODE:
    ast->arguments = va_args(args, node*);
    ast->statements = va_args(args, node*);
    break;
  case EXPRESSION_VAR:
    ast->var = va_args(args,node*);
    break;
  case NESTED_EXPRESSION_NODE:
    ast->expression = va_args(args,node*);
    break;
  case IF_STATEMENT_NODE:
    ast->condition = va_args(args, node*);
    ast->bodyStatement = va_args(args, node*);
    ast->else_statement = va_args(args, node*);
    break;
  case NESTED_SCOPE_NODE:
    ast->scope = va_args(args, node*);
    break;
  case DECLARATIONS_NODE:
    ast->declarations = va_args(args, node*);
    ast->declaration = va_args(args, node*);
    break;
  case DECLARATION_NODE:
    ast->is_const = va_args(args, int);
    ast->type = va_args(args, node*);
    ast->id = va_args(args, char*);
    ast->expression = va_args(args, node*);
    break;
  default: break;
  }

  va_end(args);

  return ast;
}

void ast_free(node *ast) {

}

void ast_print(node * ast) {

}
