#include "ast.h"
#include "semantic.h"
#include <stdio.h>

int semantic_check( node *ast) {
  int result = ast_traverse(ast, &checkEnterScope, &checkSemanticsAndExitScope);
  return result;
}

void* checkEnterScope(node* ast){
  if (ast->kind == SCOPE_NODE) {
    enterNewScope();
  }
}

void* checkSemanticsAndExitScope(node* ast) {
  node_kind kind = ast->kind;

  switch (kind) {
    case SCOPE_NODE:
      //this is the post-order call - we are now exiting a scope
      exitScope();
      break;
    case UNARY_EXPRESION_NODE:
      
      break;
    case BINARY_EXPRESSION_NODE:
      break;
    case INT_NODE:
      break;
    case FLOAT_NODE:
      break;
    case VAR_NODE:
      break;
    case FUNCTION_NODE:
      break;
    case CONSTRUCTOR_NODE:
      break;
    case TYPE_NODE:
      break;
    case BOOL_NODE:
      break;
    case STATEMENT_NODE:
      break;
    case STATEMENTS_NODE:
      break;
    case ARGUMENTS_NODE:
      break;
    case EXPRESSION_VAR:
      break;
    case NESTED_EXPRESSION_NODE:
      break;
    case IF_STATEMENT_NODE:
      break;
    case NESTED_SCOPE_NODE:
      break;
    case DECLARATIONS_NODE:
      break;
    case DECLARATION_NODE:
      break;
    default: 
    break;
  }
}