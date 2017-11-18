
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
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),
  EXPRESSION_NODE       = (1 << 2),
  UNARY_EXPRESION_NODE  = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE= (1 << 2) | (1 << 4),
  INT_NODE              = (1 << 2) | (1 << 5), 
  FLOAT_NODE            = (1 << 2) | (1 << 6),
  IDENT_NODE            = (1 << 2) | (1 << 7),
  VAR_NODE              = (1 << 2) | (1 << 8),
  FUNCTION_NODE         = (1 << 2) | (1 << 9),
  CONSTRUCTOR_NODE      = (1 << 2) | (1 << 10),
  TYPE_NODE             = (1 << 2) | (1 << 11),
  BOOL_NODE             = (1 <<2 ) | (1 << 12),

  STATEMENT_NODE        = (1 << 1),  
  STATEMENTS_NODE       = (1 << 1) | (1 << 2),

  ARGUMENTS_NODE        = (1 << 1) | (1 << 8),
  EXPRESSION_VAR        = (1 << 1) | (1 << 9),
  NESTED_EXPRESSION_NODE= (1 << 1) | (1 << 10),
  IF_STATEMENT_NODE     = (1 << 1) | (1 << 11),
  WHILE_STATEMENT_NODE  = (1 << 1) | (1 << 12),
  ASSIGNMENT_NODE       = (1 << 1) | (1 << 13),
  NESTED_SCOPE_NODE     = (1 << 1) | (1 << 14),
  DECLARATIONS_NODE     = (1 << 1) | (1 << 15),
  DECLARATION_NODE      = (1 << 15)
} node_kind;

struct node_ {

  // an example of tagging each node with a type
  node_kind kind;

  union {
    struct {
      node *declarations;
      node * statements;
    } scope;

    struct {
      
    } expression;    
    
    struct {
      int op;
      node *right;
    } unary_expr;
    
    struct {
      int op;
      node *left;
      node *right;
    } binary_expr;


    struct {
      int val;
    } int_node;

    struct {
      float val;
    } float_node;

    struct {
      int is_array;
      int array_index;
      char* id;
    } var_node;

    struct {
      node* arguments;
      int function;
    } function_node;

    struct {
      node* type;
      node* arguments;
    }constructor_node;
    
    struct {
      int length;
      int token_type;
    }type_node;

    struct {
      int value;
    }bool_node;

    struct {
      //strictly for assignment
      node* variable;
      node* expression;
    }statement_node;

    struct {
      node* statements;
      node* statement;
    }statements_node;

    struct {
      node* arguments;
      node* expression;
    }arguments_node;

    struct {
      node* var;
    }expression_var_node;

    struct {
      node* expression;
    }nested_expression_node;

    struct {
      node* condition;
      node* bodyStatement;
      node* else_statement;
    }if_statement_node;

    struct {
      node* scope;
    }nested_scope_node;

    struct {
      node* declarations;
      node* declaration;
    }declarations_node;
    
    struct {
      int is_const;
      node* type;
      char* id;
      node* expression;
    }declaration_node;

  };
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);

#endif /* AST_H_ */
