#include "ast.h"
#include "semantic.h"
#include <stdio.h>

#define FLOAT_T 258
#define INT_T 259
#define BOOL_T 260
#define CONST 261
#define FALSE_C 262
#define TRUE_C 263
#define FUNC 264
#define IF 265
#define ELSE 266
#define AND 267
#define OR 268
#define NEQ 269
#define EQ 270
#define LEQ 271
#define GEQ 272
#define VEC_T 273
#define BVEC_T 274
#define IVEC_T 275
#define FLOAT_C 276
#define INT_C 277
#define ID 278
#define UMINUS 279
#define WITHOUT_ELSE 280
#define WITH_ELSE 281


void checkEnterScope(node* ast){
  if (ast->kind == SCOPE_NODE) {
    enterNewScope();
  }
}

int binaryOpMatchesBoolean(int op) {
  if (
      (op == AND) || 
      (op == OR)   || 
      (op == GEQ) ||
      (op == LEQ) ||
      (op == '<') ||
      (op == '>')
    ){
      return 1;
    } else { 
      return 0;
    }
}

char* typeCodeToString(int type) {
  switch (type) {
    case BOOL_T:
      return "BOOL";
      break;
    case INT_T:
      return "INT";
      break;
    case FLOAT_T:
      return "FLOAT";
      break;
    case VEC_T:
      return "VEC";
      break;
    case IVEC_T:
      return "IVEC";
      break;
    case BVEC_T:
      return "BVEC";
      break;
    default:
      //error
      return "unrecognized return type";
      break;
  }
}


int getTypeFromExpression(node* ast) {
  node_kind kind = ast->kind;
  switch(kind) {
    case BINARY_EXPRESSION_NODE: {
      if (binaryOpMatchesBoolean(ast->binary_expr.op)) {
        return BOOL_T;
      } else {
        //recurse on inner expression - arithmatic ops have same type as operands
        return getTypeFromExpression(ast->binary_expr.left);
      }
      }break;
    case UNARY_EXPRESION_NODE: {
      if (ast->unary_expr.op == '!') {
        return BOOL_T;
      } else {
        return getTypeFromExpression(ast->unary_expr.right);
      }
      }break;
    case FUNCTION_NODE: {
      int argType = getTypeFromExpression(ast->function_node.arguments->arguments_node.expression);
      switch(ast->function_node.function) {
        case 0: { //dp3
          if (argType == VEC_T) {
            return FLOAT_T;
          } else if (argType = IVEC_T) {
            return INT_T;
          }
          }break;
        case 1: { //LIT
          return VEC_T;
          }break;
        case 2: { //RSQ
          return FLOAT_T;
          }break;
      }
      }break;
    case CONSTRUCTOR_NODE: {
      return ast->constructor_node.type->type_node.token_type;
      }break;
    case BOOL_NODE: {
      return BOOL_T;
      }break;
    case INT_NODE: {
      return INT_T;
      }break;
    case FLOAT_NODE: {
      return FLOAT_T;
      }break;
    case NESTED_EXPRESSION_NODE: {
      return getTypeFromExpression(ast->nested_expression_node.expression);
      }break;
    case EXPRESSION_VAR: {
      return getTypeFromExpression(ast->expression_var_node.var);
      }break;
    case VAR_NODE: {
      return getVariableDetails(ast->var_node.id);
    }
    default: {
      //error 
      printf("error in getting type");
      return -1;
      }break;
  }
}



void checkSemanticsAndExitScope(node* ast) {
  node_kind kind = ast->kind;
  int opKind;
  int type;
  switch (kind) {
    case SCOPE_NODE:
      //this is the post-order call - we are now exiting a scope
      exitScope();
      break;

    //OPERATORS  
    case UNARY_EXPRESION_NODE: 
      /*
      TODO: 
      1) Operator matches operand type
      2) Arithmatic: -
      3) Logical: !
      */

      opKind = ast->unary_expr.op;
      type = getTypeFromExpression(ast->unary_expr.right);      
      if (opKind == '-') {
        //confirm right side is arithmatic value
        if (type == BOOL_T || type == BVEC_T) {
          printf("Semantic error: Expecting arithmatic operand but got type: %s", 
            typeCodeToString(type));
        }
      } else if (opKind == '!')
      {
        if (type != BOOL_T && type != BVEC_T) {
          printf("Semantic error: Expecting boolean operand but got type %s,",
          typeCodeToString(type));
        }
      } else {
        //should never go here
        printf ("major error");
      }


      break;
    case BINARY_EXPRESSION_NODE:
      /*
      1) Operator match operand types
      2) Arithmatic: +, -, /, *, ^, 
      3) Logical: &&, ||
      4) Comparison: <, <=, >, >=, ==, !=
      */
      break;
    case VAR_NODE:
      //check if accessing predefined value
      break;
    case FUNCTION_NODE:
      /*
      float rsq ( float ) ;
      float rsq ( int ) ;
      float dp3 ( vec4 , vec4 ) ;
      float dp3 ( vec3 , vec3 ) ;
      int dp3 (ivec4 , ivec4) ;
      int dp3 (ivec3, ivec3) ;
      vec4 lit (vec4) ;
      */
      break;
    case CONSTRUCTOR_NODE:
      break;
    case STATEMENT_NODE:
      break;
    case ARGUMENTS_NODE:
      break;
    case EXPRESSION_VAR:
      break;
    case IF_STATEMENT_NODE:
      break;
    case DECLARATION_NODE:
      break;
    default: 
    break;
  }
}


int semantic_check( node *ast) {
  ast_traverse(ast, &checkEnterScope, &checkSemanticsAndExitScope);
  int result =1 ;
  return result;
}