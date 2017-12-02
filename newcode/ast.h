
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>

// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;

typedef enum {
  UNKNOWN                          = 0,
  
  SCOPE_NODE                       = 1,

  ASSIGNMENT_STATEMENT_NODE        = 2,
  IF_STATEMENT_NODE                = 3,
  IF_ELSE_STATEMENT_NODE           = 4,
  STATEMENT_SCOPE_NODE             = 5,
  
  EXPRESSION_NODE                  = 6,
  UNARY_EXPRESSION_NODE            = 7,
  BINARY_EXPRESSION_NODE           = 8,
  INT_NODE                         = 9,
  FLOAT_NODE                       = 10,
  IDENT_NODE                       = 11,
  EXPRESSION_VAR_NODE              = 12,
  VAR_NODE                         = 13,
  FUNCTION_NODE                    = 14,
  CONSTRUCTOR_NODE                 = 15,
  
  TYPE_NODE                        = 16,
  BOOL_NODE                        = 17,
    
  PAREN_EXPRESSION_NODE            = 18,
  ARGUMENTS_NODE                   = 19,

  INDEXING_NODE                    = 20,
  STATEMENTS_NODE                  = 21,
  DECLARATIONS_NODE                = 22,
  DECLARATION_NODE                 = 23,
  ASSIGNMENT_DECLARATION_NODE      = 24,
  CONST_ASSIGNMENT_NODE            = 25,
} node_kind;

struct node_ {
  node_kind kind;
  
  struct {
    int type_name;
    int vec;
  } type;

  union {
    struct {
        node* declarations;
        node* statements;
    } scope;
    
    struct {
        node* declarations;
        node* declaration;
    } declarations;
      
    struct {
        node* statements;
        node* statement;
    } statements;
      
    struct {
        node* type;
        char* identifier;
    } declaration;
      
    struct {
        node* type;
        char* identifier;
        node* expression;
    } assignment_declaration;
      
    struct {
        node* type;
        char* identifier;
        node* expression;
    } const_assignment;
    
    struct {
        node* left;
        node* right;
    } assignment_statement;
    
    struct {
        node* condition;
        node* then_expression;
        node* else_expression;
    } if_else_statement;
      
    struct {
        node* condition;
        node* then_expression;
    } if_statement;
      
    struct {
        node* scope;
    } statement_scope;
      
    struct {
      node* type;
      node* args;
    } constructor;
  
    struct {
      int function_name;
      node *args;
    } function;

    struct {
        node* right;
        int op;
    } unary_expr;
      
    struct {
      node* left;
      node* right;
      int op;
    } binary_expr;
      
    struct {
        int bool_num;
    } bool_c;
      
    struct {
        int int_num;
    } int_c;
      
    struct {
        double float_num;
    } float_c;
      
    struct {
        node* expression;
    } paren_expression;
      
    struct {
        char* identifier;
    } variable;

    struct {
        node* variable;
    } expression_var_node;
      
    struct {
        char* identifier;
        int index;
    } array_indexing;
      
    struct {
        node* args;
        node* expression;
    } arguments;
    // etc.
  };
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast, int level);

char const* get_function(int func_id);
char const* get_operation(int op);
char const* get_type(int type_id, int vec_id);

#endif /* AST_H_ */
