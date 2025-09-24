#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "grammar.h"
#include <stdbool.h> 

typedef struct Expression Expr;
typedef struct DynamicListExpr DynamicList;
typedef struct HashMapObj HashMap;
typedef struct TypeFunctionCall FunctionCall;
typedef struct StatementObj Statement;
typedef struct IfStatementObj IfStatement;
typedef struct FunctionDefObj FunctionDef;
typedef struct EnumDefObj EnumDef;
typedef struct AttributeObj Attribute_t;
typedef struct ProgramBody_t ProgramBody;
typedef struct UntilLoopObj UntilLoop;

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
  HASH_MAP,
  FUNCTION_CALL,
  ATTRIBUTE_LIST
} ParsedToken;

typedef enum {
  VARIABLE_DEC, CONSTANT_DEC,
  ASSIGNMENT_EXP, EXPRESSION,
  IF_CONDITIONAL, 
  FUNCTION_DEFINITION,
  RETURN_EXPRESSION,
  ENUM_DEFINITION,
  UNTIL_LOOP
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

typedef struct IfStatementObj {
  Expr* condition;
  ProgramBody* body;
  ProgramBody* else_body;
} IfStatement;

typedef struct EnumDefObj {
  NamedExpr enum_name;
  uint64_t no_of_constants;
  NamedExpr** constants;
  Expr** values;
} EnumDef;

typedef struct {
  ParsedToken p_tokens;
  Expr* expression;
} ArgumentObj;

typedef struct FunctionDefObj {
  NamedExpr function_name;
  uint64_t arg_length;
  NamedExpr** arg_list;
  ProgramBody* body;
} FunctionDef;

typedef struct UntilLoopObj {
  Expr* condition;
  ProgramBody* body;
} UntilLoop;

typedef struct {
  Expr* expression;
} ReturnObj;

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

typedef struct StatementObj {
  ProgramSyntaxes expression_type;
  union { 
    VariableDec* var_declaration;
    AssignmentExpr* assignment_exp;
    Expr* expression;
    IfStatement* if_statement;
    ReturnObj* return_obj;
    FunctionDef* func_def;
    EnumDef* enum_def; 
    UntilLoop* until_loop;
  } value;
} Statement;  // every line of code is an some instruction!

typedef struct ProgramBody_t {
  uint64_t length;
  Statement** statements;
} ProgramBody;

ArgumentObj* create_arg_object(ParsedToken p_token);
IfStatement* parse_if_statement(Lexeme** lexeme_list,uint64_t top,uint64_t* index);
UntilLoop* parse_until_loopstatement(Lexeme** lexeme_list,uint64_t top,uint64_t* index);
void parse_exp_binary(Expr** expr_stack, int64_t* stack_top, Expr* exp_obj, Operators op);
void parse_exp_unary(Expr** expr_stack,int64_t* stack_top,Expr* exp_obj,Operators op);
uint64_t operator_precedance(Tokens token);
EnumDef* parse_enum_definition(Lexeme** lexeme_list,uint64_t top,uint64_t* index);
AssignmentExpr* create_assignment_expr();
NamedExpr* create_named_expr(char* name);
ConstantObj* create_constant(ConstantTypes const_type, char* name);
BinaryExpression* create_binary_exp(Operators op);
UnaryExpression* create_unary_exp(Operators op);
VariableDec* create_variable_dec();
FunctionDef* parse_function_definition(Lexeme** lexeme_list,uint64_t top,uint64_t* index);
FunctionCall* parse_function_call(Lexeme** lexeme_list,uint64_t top,uint64_t* index);
DynamicList* parse_dynamic_list(Lexeme** lexeme_list, uint64_t top, uint64_t* index);
Expr* parse_expression(Lexeme** lexeme_list, uint64_t top, uint64_t *index);
ProgramBody* body_parser(Lexeme** lexeme_list,uint64_t top,uint64_t* offset, bool inside_block);
ProgramBody* parse(Lexeme** lexeme_list, uint64_t top);

#endif

