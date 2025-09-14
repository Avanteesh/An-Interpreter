#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "grammar.h"
#include <stdbool.h> 

typedef struct Expression Expr;

typedef enum {
  PLUS, MINUS, 
  MULTIPLY,
  DIVIDE, POWER, MODULO,
  FLOOR_DIVISION,
  UNARY_PLUS, UNARY_MINUS,
  UNARY_FLIP
} Operators;

typedef enum {
  C_NUMBER, C_STRING
} ConstantTypes;

typedef enum {
  BINARY_EXP,
  UNARY_EXP,
  NAMED_EXP,
  EXPR, _NUMBER,
  STRING, BOOL
} ParsedToken;

typedef enum {
  VARIABLE_DEC, CONSTANT_DEC,
  ASSIGNMENT_EXP
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
} BinaryExpression;

typedef struct {
  Operators operation;
  Expr* operand;
} UnaryExpression;

typedef struct {
  bool is_constant;  // a variable declaration is constant if true!
  NamedExpr* target;
  Expr* value;
} VariableDec;

typedef struct {
  NamedExpr* target;
  Expr* value;
} AssignmentExpr;

typedef struct Expression {
  ParsedToken p_tokens;
  union {
    struct {
      char* value;
    } constant_obj;
    NamedExpr named_expr;
    BinaryExpression* binary_exp;
    UnaryExpression* unary_exp;
  } value;
} Expr;

typedef struct {
  ProgramSyntaxes expression_type;
  union { 
    VariableDec* var_declaration;
    AssignmentExpr* assignment_exp;
  } value;
} Instruction;

AssignmentExpr* create_assignment_expr();
NamedExpr* create_named_expr(char* name);
ConstantObj* create_constant(ConstantTypes const_type, char* name);
BinaryExpression* create_binary_exp(Operators op);
UnaryExpression* create_unary_exp(Operators op);
VariableDec* create_variable_dec();
Expr* parse_expression(Lexeme** lexeme_list, uint64_t top, uint64_t *index);
void parse(Lexeme** lexeme_list, uint64_t top);

#endif

