#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "grammar.h"
#include <stdbool.h> 

typedef struct Expression Expr;
typedef struct DynamicListExpr DynamicList;
typedef struct TypeFunctionCall FunctionCall;

typedef enum {
  PLUS, MINUS, 
  MULTIPLY, 
  DIVIDE, POWER, MODULO,
  FLOOR_DIVISION,
  UNARY_PLUS, UNARY_MINUS,
  UNARY_FLIP, LOGICAL_AND,
  LOGICAL_OR,LOGICAL_XOR,
  LOGICAL_NOT,BITWISE_AND,
  BITWISE_OR,BITWISE_XOR,
  ARITHMATIC_LSHIFT,
  ARITHMATIC_RSHIFT,
  GREATER_THAN,LESS_THAN,
  GREATER_THAN_OR_EQUAL,
  LESS_THAN_OR_EQUAL,
  BANG_EQUALITY,NOT_EQUALITY
} Operators;

typedef enum {
  C_NUMBER, C_STRING
} ConstantTypes;

typedef enum {
  BINARY_EXP,
  UNARY_EXP,
  NAMED_EXP,
  EXPR, _NUMBER,
  STRING, BOOL,
  DYNAMIC_LIST,
  FUNCTION_CALL
} ParsedToken;

typedef enum {
  VARIABLE_DEC, CONSTANT_DEC,
  ASSIGNMENT_EXP, EXPRESSION
} ProgramSyntaxes;

typedef struct {
  char* var_name;
} NamedExpr;

typedef struct {
  ConstantTypes type;
  char* value;
} ConstantObj;

typedef struct {
 Operators operation;
 Expr* left;
 Expr* right;
} BinaryExpression;  // binary expression (can be arithmatic or boolean!)

typedef struct {
  Operators operation;
  Expr* operand;
} UnaryExpression;  // unary operations!

typedef struct {
  bool is_constant;// a variable declaration is constant if true!
  NamedExpr* target;
  Expr* value;
} VariableDec;

typedef struct {
  NamedExpr* target;
  Expr* value;
} AssignmentExpr;

typedef struct {
  ParsedToken p_tokens;
  Expr* expression;
} ArgumentObj;

typedef struct TypeFunctionCall {
  NamedExpr function_name;
  uint64_t arg_length;
  ArgumentObj** arg_list;
} FunctionCall;

typedef struct DynamicListExpr {
  uint64_t length;
  ArgumentObj** arg_list;
} DynamicList;   // representation of List data structure!

typedef struct Expression {
  ParsedToken p_tokens;
  union {
    struct {
      char* value;
    } constant_obj;
    NamedExpr named_expr;
    BinaryExpression* binary_exp;
    UnaryExpression* unary_exp;
    DynamicList* dynamic_list;
    FunctionCall* function_call;
  } value;
} Expr;

typedef struct {
  ProgramSyntaxes expression_type;
  union { 
    VariableDec* var_declaration;
    AssignmentExpr* assignment_exp;
    Expr* expression;
  } value;
} Instruction;  // every line of code is an some instruction!

ArgumentObj* create_arg_object(ParsedToken p_token);
AssignmentExpr* create_assignment_expr();
NamedExpr* create_named_expr(char* name);
ConstantObj* create_constant(ConstantTypes const_type, char* name);
BinaryExpression* create_binary_exp(Operators op);
UnaryExpression* create_unary_exp(Operators op);
VariableDec* create_variable_dec();
FunctionCall* parse_function_call(Lexeme** lexeme_list,uint64_t top,uint64_t* index);
DynamicList* parse_dynamic_list(Lexeme** lexeme_list, uint64_t top, uint64_t* index);
Expr* parse_expression(Lexeme** lexeme_list, uint64_t top, uint64_t *index);
void parse(Lexeme** lexeme_list, uint64_t top);

#endif

