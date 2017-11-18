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
    ast->scope.declarations = va_arg(args, node*);
    ast->scope.statements = va_arg(args,node*);
    break;
  case EXPRESSION_NODE:
    break;
  case UNARY_EXPRESION_NODE:
    ast->unary_expr.op = va_arg(args, int);
    ast->unary_expr.right = va_arg(args,node*);
    break;
  case BINARY_EXPRESSION_NODE:
    ast->binary_expr.op = va_arg(args, int);
    ast->binary_expr.left = va_arg(args, node *);
    ast->binary_expr.right = va_arg(args, node *);  
    break;
  case INT_NODE:
    ast->int_node.val = va_arg(args,int);
    break;
  case FLOAT_NODE:
    ast->float_node.val = va_arg(args,float);
    break;
  case VAR_NODE:
    ast->var_node.id = va_arg(args, char*);
    ast->var_node.is_array = va_arg(args, int);
    ast->var_node.array_index = va_arg(args, int);
    break;
  case FUNCTION_NODE:
    ast->function_node.function = va_arg(args, int);
    ast->function_node.arguments = va_arg(args, node*);
    break;
  case CONSTRUCTOR_NODE:
    ast->constructor_node.type = va_arg(args,node*);
    ast->constructor_node.arguments = va_arg(args, node*);
    break;
  case TYPE_NODE:
    ast->type_node.token_type = va_arg(args, int);
    ast->type_node.length = va_arg(args, int);
    break;
  case BOOL_NODE:
    ast->bool_node.value = va_arg(args,int);
    break;
  case STATEMENT_NODE:
    ast->statement_node.variable = va_arg(args, node*);
    ast->statement_node.expression = va_arg(args, node*);
    break;
  case STATEMENTS_NODE:
    ast->statements_node.statements = va_arg(args,node*);
    ast->statements_node.statement = va_arg(args, node*);
    break;
  case ARGUMENTS_NODE:
    ast->arguments_node.arguments = va_arg(args, node*);
    ast->arguments_node.statements = va_arg(args, node*);
    break;
  case EXPRESSION_VAR:
    ast->expression_var_node.var = va_arg(args,node*);
    break;
  case NESTED_EXPRESSION_NODE:
    ast->nested_expression_node.expression = va_arg(args,node*);
    break;
  case IF_STATEMENT_NODE:
    ast->if_statement_node.condition = va_arg(args, node*);
    ast->if_statement_node.bodyStatement = va_arg(args, node*);
    ast->if_statement_node.else_statement = va_arg(args, node*);
    break;
  case NESTED_SCOPE_NODE:
    ast->nested_scope_node.scope = va_arg(args, node*);
    break;
  case DECLARATIONS_NODE:
    ast->declarations_node.declarations = va_arg(args, node*);
    ast->declarations_node.declaration = va_arg(args, node*);
    break;
  case DECLARATION_NODE:
    ast->declaration_node.is_const = va_arg(args, int);
    ast->declaration_node.type = va_arg(args, node*);
    ast->declaration_node.id = va_arg(args, char*);
    ast->declaration_node.expression = va_arg(args, node*);
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
