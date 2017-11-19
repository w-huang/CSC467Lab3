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
      ast->float_node.val = va_arg(args,double);
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
      ast->arguments_node.expression = va_arg(args, node*);
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
      ast->declaration_node.id = va_arg(args, char*);    
      ast->declaration_node.type = va_arg(args, node*);
      ast->declaration_node.expression = va_arg(args, node*);
      break;
    default: break;
  }

  va_end(args);

  return ast;
}

void ast_free(node *ast) {

    if (ast == NULL) {
      //leaf NULL node, do nothing;
      return;
    }

    node_kind kind = ast->kind;
    //free children NODES first. Depends on what type of node it is
    switch(kind) {
      case SCOPE_NODE:
        ast_free(ast->scope.declarations);
        ast_free(ast->scope.statements);
        break;
      case UNARY_EXPRESION_NODE:
        ast_free(ast->unary_expr.right);
        break;
      case BINARY_EXPRESSION_NODE:
        ast_free(ast->binary_expr.left);
        ast_free(ast->binary_expr.right);
        break;
      case INT_NODE:
        break;
      case FLOAT_NODE:
        break;
      case VAR_NODE:
        free(ast->var_node.id);
        break;
      case FUNCTION_NODE:
        ast_free(ast->function_node.arguments);
        break;
      case CONSTRUCTOR_NODE:
        ast_free(ast->constructor_node.type);
        ast_free(ast->constructor_node.arguments);
        break;
      case TYPE_NODE:
        break;
      case BOOL_NODE:
        break;
      case STATEMENT_NODE:
        ast_free(ast->statement_node.variable);
        ast_free(ast->statement_node.expression);
        break;
      case STATEMENTS_NODE:
        ast_free(ast->statements_node.statements);
        ast_free(ast->statements_node.statement);
        break;
      case ARGUMENTS_NODE:
        ast_free(ast->arguments_node.arguments);
        ast_free(ast->arguments_node.expression);
        break;
      case EXPRESSION_VAR:
        ast_free(ast->expression_var_node.var);
        break;
      case NESTED_EXPRESSION_NODE:
        ast_free(ast->nested_expression_node.expression);
        break;
      case IF_STATEMENT_NODE:
        ast_free(ast->if_statement_node.condition);
        ast_free(ast->if_statement_node.bodyStatement);
        ast_free(ast->if_statement_node.else_statement);
        break;
      case NESTED_SCOPE_NODE:
        ast_free(ast->nested_scope_node.scope);
        break;
      case DECLARATIONS_NODE:
        ast_free(ast->declarations_node.declarations);
        ast_free(ast->declarations_node.declaration);
        break;
      case DECLARATION_NODE:
        ast_free(ast->declaration_node.type);
        ast_free(ast->declaration_node.expression);
        free(ast->declaration_node.id);
        break;
      default: 
      break;
    }

    //free self
    free(ast);
    return;
}

void ast_print(node * ast) {
  if (ast == NULL) {
    //leaf node
    //do nothing
    return;
  }
  node_kind kind = ast->kind; //important metadata

  printf("(");
  //printing is pre-order: print self first
  print_node_type(ast);

  //print children. Depends on what type of node it is
  switch(kind) {
    case SCOPE_NODE:
      ast_print(ast->scope.declarations);
      ast_print(ast->scope.statements);
      break;
    case UNARY_EXPRESION_NODE:
      printf("UNARY_OP[%d]", ast->unary_expr.op);
      ast_print(ast->unary_expr.right);
      break;
    case BINARY_EXPRESSION_NODE:
      ast_print(ast->binary_expr.left);
      printf("BINARY_OP[%d]\n", ast->binary_expr.op);
      ast_print(ast->binary_expr.right);
      break;
    case INT_NODE:
      printf("INT_NODE[%d]\n", ast->int_node.val);
      break;
    case FLOAT_NODE:
      printf("INT_NODE[%f]\n", ast->float_node.val);
      break;
    case VAR_NODE:
      printf("VAR_isArray[%d]\n", ast->var_node.is_array);
      printf("VAR_array_index[%d]\n", ast->var_node.array_index);
      printf("VAR_id: [%s]\n", ast->var_node.id);
      break;
    case FUNCTION_NODE:
      printf("FUNCTION_NAME[%d]\n", ast->function_node.function);
      ast_print(ast->function_node.arguments);
      break;
    case CONSTRUCTOR_NODE:
      ast_print(ast->constructor_node.type);
      ast_print(ast->constructor_node.arguments);
      break;
    case TYPE_NODE:
      printf("TYPE[%d]\n", ast->type_node.token_type);
      printf("TYPE_LENGTH[%d]\n", ast->type_node.token_type);
      break;
    case BOOL_NODE:
      if (ast->bool_node.value != 0) {
        printf("BOOL_NODE[TRUE]\n");
      } else {
        printf("BOOL_NODE[FALSE]\n");
      }
      break;
    case STATEMENT_NODE:
      ast_print(ast->statement_node.variable);
      ast_print(ast->statement_node.expression);
      break;
    case STATEMENTS_NODE:
      ast_print(ast->statements_node.statements);
      ast_print(ast->statements_node.statement);
      break;
    case ARGUMENTS_NODE:
      ast_print(ast->arguments_node.arguments);
      ast_print(ast->arguments_node.expression);
      break;
    case EXPRESSION_VAR:
      ast_print(ast->expression_var_node.var);
      break;
    case NESTED_EXPRESSION_NODE:
      ast_print(ast->nested_expression_node.expression);
      break;
    case IF_STATEMENT_NODE:
      ast_print(ast->if_statement_node.condition);
      ast_print(ast->if_statement_node.bodyStatement);
      ast_print(ast->if_statement_node.else_statement);
      break;
    case NESTED_SCOPE_NODE:
      ast_print(ast->nested_scope_node.scope);
      break;
    case DECLARATIONS_NODE:
      ast_print(ast->declarations_node.declarations);
      ast_print(ast->declarations_node.declaration);
      break;
    case DECLARATION_NODE:
      printf("DECLARATION_ISCONST[%d]\n", ast->declaration_node.is_const); //isconst
      ast_print(ast->declaration_node.type);
      printf("DECLARATION_NAME[%s]\n", ast->declaration_node.id); //id
      ast_print(ast->declaration_node.expression);
      break;
    default: 
    break;
  }

  printf(")");

}

void print_node_type(node* ast) {
  node_kind kind = ast->kind;
  switch(kind) {
    case SCOPE_NODE:
      printf("SCOPE\n");
      break;
    case UNARY_EXPRESION_NODE:
      printf("UNARY_EXPRESSION\n");
      break;
    case BINARY_EXPRESSION_NODE:
      printf("BINARY_EXPRESSION\n");
      break;
    case INT_NODE:
      printf("INT\n");
      break;
    case FLOAT_NODE:
      printf("FLOAT\n");
      break;
    case VAR_NODE:
      printf("VAR\n");
      break;
    case FUNCTION_NODE:
      printf("FUNCTION\n");
      break;
    case CONSTRUCTOR_NODE:
      printf("CONSTRUCTOR\n");
      break;
    case TYPE_NODE:
      printf("TYPE\n");
      break;
    case BOOL_NODE:
      printf("BOOL\n");
      break;
    case STATEMENT_NODE:
      printf("STATEMENT\n");
      break;
    case STATEMENTS_NODE:
      printf("STATEMENTS\n");
      break;
    case ARGUMENTS_NODE:
      printf("ARGUMENTS\n");
      break;
    case EXPRESSION_VAR:
      printf("EXPRESSION-VARIABLE\n");
      break;
    case NESTED_EXPRESSION_NODE:
      printf("NESTED_EXPRESSION\n");
      break;
    case IF_STATEMENT_NODE:
      printf("IF_STATEMENT\n");
      break;
    case NESTED_SCOPE_NODE:
      printf("NESTED_SCOPE\n");
      break;
    case DECLARATIONS_NODE:
      printf("DECLARATIONS\n");
      break;
    case DECLARATION_NODE:
      printf("DECLARATION\n");
      break;
    default: 
      printf("UNCAUGHT NODE TYPE: ");
      printf("%d\n", kind);
    break;
    }
}