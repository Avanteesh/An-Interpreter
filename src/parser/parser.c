#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "grammar.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  bool is_tokenized;
  union {
    Lexeme* token;
    Expr* parsed_expression;
  } value;
} TokenizedObject;

NamedExpr* create_named_expr(char* name)   {
  NamedExpr* nm = (NamedExpr*)malloc(sizeof(NamedExpr));
  if (nm == NULL)  {
    fprintf(stderr, "ERROR! recompile!\n");
    return NULL;
  }
  nm->var_name = (char*)malloc(sizeof(char)*strlen(name));
  strcpy(nm->var_name, name);
  return nm;
}

ConstantObj* create_constant(ConstantTypes const_types, char* name)   {
  ConstantObj* const_obj = (ConstantObj*)malloc(sizeof(ConstantObj));
  if (const_obj == NULL)   {
   fprintf(stderr, "ERROR recompile!\n");
   return NULL;
  }
  const_obj->type = const_types;
  const_obj->value = malloc(sizeof(char)*strlen(name));
  strcpy(const_obj->value, name);
  return const_obj;
}

BinaryExpression* create_binary_exp(Operators op)  {
  BinaryExpression* binexp = (BinaryExpression*)malloc(sizeof(BinaryExpression));
  if (binexp == NULL)   {
    fprintf(stderr, "ERROR in creation of binary expression! recompile!");
    return NULL;
  }
  binexp->operation = op;
  return binexp;
}

UnaryExpression* create_unary_exp(Operators op)  {
  UnaryExpression* unary_exp = (UnaryExpression*)malloc(sizeof(UnaryExpression));
  if (unary_exp == NULL)   {
   fprintf(stderr, "ERROR in creation of Unary expression! recompile!");
   return NULL;
  }
  unary_exp->operation = op;
  return unary_exp;
}

AssignmentExpr* create_assignment_expr()  {
  AssignmentExpr* as_exp = (AssignmentExpr*)malloc(sizeof(AssignmentExpr));
  if (as_exp == NULL)  {
   fprintf(stderr, "ERROR in creating assignment expression! recompile!");
   return NULL;
  }
  as_exp->target = NULL;
  as_exp->value = NULL;
}

VariableDec* create_variable_dec()  {
  VariableDec* var_dec = (VariableDec*)malloc(sizeof(VariableDec));
  if (var_dec == NULL)  {
    fprintf(stderr, "ERROR! recompile!");
    return NULL;
  }
  var_dec->is_constant = false;
  var_dec->target = NULL;
  var_dec->value = NULL;
  return var_dec;
}

uint64_t operator_precedance(Tokens token)   {
  switch(token)  {
    case LEFT_BRACE: case RIGHT_BRACE: return 14;
    case POWER_OP: return 13;
    case UNARY_PLUS_OP: case UNARY_MINUS_OP: 
    case UNARY_FLIP_OP: return 12;
    case PRODUCT_OP : case DIVIDE_OP: 
    case MODULUS_OP: case FLOOR_DIV_OP: return 11;
    case PLUS_OP: case MINUS_OP: return 10;
    case ARITH_LSHIFT: case ARITH_RSHIFT: return 9;
    case BIT_AND_OP: return 8;
    case BIT_XOR_OP: return 7;
    case BIT_OR_OP: return 6;
    case GREATER_THAN_OP: case LESS_THAN_OP:
    case GREATER_THAN_OR_EQUAL_OP: case LESS_THAN_OR_EQUAL_OP:
    case EQUALITY_OP: case NOT_EQUAL_OP: return 5;
    case LOG_NOT_OP: return 4;
    case LOG_AND_OP: return 3;
    case LOG_XOR_OP: return 2;
    case LOG_OR_OP: return 1;
    default: return 0;
  } 
}

void parse_exp_binary(Expr** expr_stack,int64_t* stack_top,Expr* exp_obj,Operators op)  {
  Expr *first = expr_stack[(*stack_top)], *second = expr_stack[(*stack_top)-1];
  expr_stack[(*stack_top)--] = NULL;
  expr_stack[(*stack_top)--] = NULL; 
  exp_obj->p_tokens = BINARY_EXP;
  exp_obj->value.binary_exp = create_binary_exp(op);
  exp_obj->value.binary_exp->left = (Expr*)malloc(sizeof(Expr));
  exp_obj->value.binary_exp->right = (Expr*)malloc(sizeof(Expr));
  exp_obj->value.binary_exp->left->p_tokens = second->p_tokens;
  exp_obj->value.binary_exp->right->p_tokens = first->p_tokens;
  if (second->p_tokens == _NUMBER || second->p_tokens == STRING)  {
   exp_obj->value.binary_exp->left->value.constant_obj = second->value.constant_obj;
  } else if (second->p_tokens == BINARY_EXP)  {
   exp_obj->value.binary_exp->left->value.binary_exp = second->value.binary_exp;
  } else if (second->p_tokens == NAMED_EXP)  {
   exp_obj->value.binary_exp->left->value.named_expr = second->value.named_expr;
  } else if (second->p_tokens == DYNAMIC_LIST)  {
   exp_obj->value.binary_exp->left->value.dynamic_list = second->value.dynamic_list;
  } else if (second->p_tokens == UNARY_EXP)  {
   exp_obj->value.binary_exp->left->value.unary_exp = second->value.unary_exp;
  } 
  if (first->p_tokens == _NUMBER || first->p_tokens == STRING) {
   exp_obj->value.binary_exp->right->value.constant_obj = first->value.constant_obj;
  } else if (first->p_tokens == BINARY_EXP)  {
   exp_obj->value.binary_exp->right->value.binary_exp = first->value.binary_exp;
  } else if (first->p_tokens == NAMED_EXP) {
   exp_obj->value.binary_exp->right->value.named_expr = first->value.named_expr;
  } else if (first->p_tokens == DYNAMIC_LIST) {
   exp_obj->value.binary_exp->right->value.dynamic_list = first->value.dynamic_list;
  } else if (first->p_tokens == UNARY_EXP)  {
   exp_obj->value.binary_exp->right->value.unary_exp = first->value.unary_exp;
  }
  expr_stack[++(*stack_top)] = exp_obj;
}

void parse_exp_unary(Expr** expr_stack, int64_t* stack_top,Expr* exp_obj,Operators op) {
  Expr* f_operand = expr_stack[(*stack_top)];
  expr_stack[(*stack_top)--] = NULL;
  exp_obj->p_tokens = UNARY_EXP;
  exp_obj->value.unary_exp = create_unary_exp(op);
  exp_obj->value.unary_exp->operand = (Expr*)malloc(sizeof(Expr));
  exp_obj->value.unary_exp->operand->p_tokens = f_operand->p_tokens;
  if (f_operand->p_tokens == _NUMBER)  {
    exp_obj->value.unary_exp->operand->value.constant_obj = f_operand->value.constant_obj;
  } else if (f_operand->p_tokens == BINARY_EXP)  {
    exp_obj->value.unary_exp->operand->value.binary_exp = f_operand->value.binary_exp;
  } else if (f_operand->p_tokens == NAMED_EXP)  {
    exp_obj->value.unary_exp->operand->value.named_expr = f_operand->value.named_expr;
  } else if (f_operand->p_tokens == UNARY_EXP)  {
    exp_obj->value.unary_exp->operand->value.unary_exp = f_operand->value.unary_exp;
  }
  expr_stack[++(*stack_top)] = exp_obj;
}

ArgumentObj* create_arg_object(ParsedToken p_token)  {
  ArgumentObj* arg_obj = (ArgumentObj*)malloc(sizeof(ArgumentObj));
  if (arg_obj == NULL)  {
    fprintf(stderr, "Cannot create argument object!\n");
    exit(-1);
  }
  arg_obj->p_tokens = p_token;
  return arg_obj;
}

DynamicList* parse_dynamic_list(Lexeme** lexeme_list, uint64_t top, uint64_t* index)  {
  DynamicList* list_obj = (DynamicList*)malloc(sizeof(DynamicList));
  list_obj->length = 0;
  int64_t list_top = -1;
  list_obj->arg_list = (ArgumentObj**)malloc(sizeof(ArgumentObj*)*1);
  bool exit_loop = false;
  while (true)  {
    switch(lexeme_list[(*index)]->lexeme_type) {
      case DYNAMIC_LIST_RIGHT_BRACE:
	(*index)++;
	exit_loop = true;
	break;
      case COMMA:
	(*index)++;
	break;
      case LINE_END:
	(*index)++;
	break;
      case NUMBER: case STRING_LITERAL: case NAMED_LEXEME:
      case LEFT_BRACE: case DYNAMIC_LIST_LEFT_BRACE: case
	PLUS_OP: case MINUS_OP: case UNARY_FLIP_OP: case LOG_NOT_OP:
	Expr* exp = parse_expression(lexeme_list,top,&(*index));
	list_top++;
	list_obj->length++;
	if (lexeme_list[(*index)]->lexeme_type == SEMICOLON)
          (*index)--;
	if (list_top > 0)  {
          ArgumentObj** new_lis = realloc(
	   list_obj->arg_list, sizeof(ArgumentObj*)*list_top+1
	  );
	  list_obj->arg_list = new_lis;
	}
	list_obj->arg_list[list_top] = create_arg_object(exp->p_tokens);
	list_obj->arg_list[list_top]->expression = exp;
	break;
    }
    if (exit_loop) break;
  }
  return list_obj;
}

static bool is_comparision_operator(Tokens symbol)  {
  switch(symbol) { 
     case GREATER_THAN_OP:case LESS_THAN_OP:
     case GREATER_THAN_OR_EQUAL_OP: case LESS_THAN_OR_EQUAL_OP:
     case EQUALITY_OP: case NOT_EQUAL_OP: 
    return true;      
  }
  return false;
}

Expr* parse_expression(Lexeme** lexeme_list, uint64_t top, uint64_t* index)  {
  TokenizedObject* stack = malloc(sizeof(TokenizedObject)*top);
  TokenizedObject* post_exp = malloc(sizeof(TokenizedObject)*top);
  int64_t stack_top = -1, post_top = -1;
  while (true)  {
    if (lexeme_list[*index]->lexeme_type == SEMICOLON || lexeme_list[*index]->lexeme_type == LINE_END || lexeme_list[*index]->lexeme_type == COMMA || lexeme_list[*index]->lexeme_type == MAP_OPERATOR)  {
       break;
    } else if (lexeme_list[*index]->lexeme_type == RESERVED_WORD) {
      if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DO) == 0) break;
      else if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DONE) == 0) break;
    } else if (lexeme_list[*index]->lexeme_type == NUMBER || lexeme_list[*index]->lexeme_type == STRING_LITERAL)  {
      post_exp[++post_top].is_tokenized = false;
      post_exp[post_top].value.token = lexeme_list[(*index)++];
    } else if (lexeme_list[(*index)]->lexeme_type == NAMED_LEXEME) {
      if (lexeme_list[(*index) + 1]->lexeme_type == LEFT_BRACE)  {
	// a named expression is a function call if next token is left bracket!
	FunctionCall* f_call = parse_function_call(lexeme_list, top, &(*index));
	Expr* funcall_exp = (Expr*)malloc(sizeof(Expr));
	funcall_exp->p_tokens = FUNCTION_CALL;
	funcall_exp->value.function_call = f_call;
	post_exp[++post_top].is_tokenized = true;
	post_exp[post_top].value.parsed_expression = funcall_exp;
      } else {
	// else it's a variable/reference!
        post_exp[++post_top].is_tokenized = false;
	post_exp[post_top].value.token = lexeme_list[(*index)++];
      }
    } 
    else if (lexeme_list[(*index)]->lexeme_type == DYNAMIC_LIST_LEFT_BRACE)  {
      (*index)++;
      DynamicList* parsed_list = parse_dynamic_list(lexeme_list, top,&(*index));
      Expr* lis_exp = (Expr*)malloc(sizeof(Expr));
      lis_exp->p_tokens = DYNAMIC_LIST;
      lis_exp->value.dynamic_list = parsed_list;
      post_exp[++post_top].is_tokenized = true;
      post_exp[post_top].value.parsed_expression = lis_exp;
    }
    else if (lexeme_list[(*index)]->lexeme_type == DYNAMIC_LIST_RIGHT_BRACE) {
      break;
    } else if (lexeme_list[(*index)]->lexeme_type == LEFT_BRACE) {
      stack[++stack_top].is_tokenized = false;
      stack[stack_top].value.token = lexeme_list[(*index)++];
    } 
    else if (lexeme_list[(*index)]->lexeme_type == RIGHT_BRACE || 
	is_comparision_operator(lexeme_list[(*index)]->lexeme_type) == true)  {
      if (stack_top > -1 && is_comparision_operator(lexeme_list[(*index)]->lexeme_type) == true) {
        while (true)  {
	  if (stack_top < 0) break;
	  else if (stack[stack_top].value.token->lexeme_type == LEFT_BRACE) break;
	  TokenizedObject lex_tok = stack[stack_top--];
          post_exp[++post_top] = lex_tok;
        }
	if (stack_top > -1) stack_top--;
      }
      if (is_comparision_operator(lexeme_list[*index]->lexeme_type) == true) {
	stack[++stack_top].is_tokenized = false;
	stack[stack_top].value.token = lexeme_list[(*index)];
      }
      (*index)++;
    } else {
       if (lexeme_list[*index]->lexeme_type == PLUS_OP)  {
         if (lexeme_list[(*index) - 1]->lexeme_type != NUMBER &&
	   lexeme_list[(*index) - 1]->lexeme_type != NAMED_LEXEME)  {
	   if (lexeme_list[(*index) - 1]->lexeme_type == LEFT_BRACE ||
		lexeme_list[(*index) - 1]->lexeme_type == ASSIGNMENT_OP)  {
             lexeme_list[*index]->lexeme_type = UNARY_PLUS_OP;
	   }
	 }
       }
       else if (lexeme_list[*index]->lexeme_type == MINUS_OP)  {
	 if (lexeme_list[(*index) - 1]->lexeme_type != NUMBER &&
	   lexeme_list[(*index) - 1]->lexeme_type != NAMED_LEXEME)  {
	   if (lexeme_list[(*index) - 1]->lexeme_type == LEFT_BRACE || 
		lexeme_list[(*index) - 1]->lexeme_type == ASSIGNMENT_OP)  {
	     lexeme_list[*index]->lexeme_type = UNARY_MINUS_OP;
	   }
	 }
       }
       if (!(lexeme_list[*index]->lexeme_type == UNARY_MINUS_OP 
	 || lexeme_list[*index]->lexeme_type == UNARY_PLUS_OP)) {	
        if (stack_top != -1)  {
          uint64_t first = operator_precedance(lexeme_list[*index]->lexeme_type);
          while ((stack_top != -1) && 
            first <= operator_precedance(stack[stack_top].value.token->lexeme_type) &&
	    stack[stack_top].value.token->lexeme_type != LEFT_BRACE)   {
            post_exp[++post_top] = stack[stack_top--];
          }
	}
      }
      stack[++stack_top].is_tokenized = false;
      stack[stack_top].value.token = lexeme_list[(*index)++];
    }
  }
  while (stack_top != -1)  
    post_exp[++post_top] = stack[stack_top--];
  free(stack);
  Expr** expr_stack = malloc(sizeof(Expr*)*post_top);
  for (uint64_t i = 0; i <= post_top; i++)   {
    Expr* exp_obj = (Expr*)malloc(sizeof(Expr));
    if (post_exp[i].is_tokenized == false)  {
      switch(post_exp[i].value.token->lexeme_type)  {
	case NUMBER: case STRING_LITERAL:
         exp_obj->p_tokens = (post_exp[i].value.token->lexeme_type == NUMBER)? _NUMBER:STRING;
         exp_obj->value.constant_obj.value = (char*)malloc(
	   sizeof(char)*strlen(post_exp[i].value.token->content)
	 );
         strcpy(exp_obj->value.constant_obj.value, post_exp[i].value.token->content);
         expr_stack[++stack_top] = exp_obj;
	 break;
        case NAMED_LEXEME:
         exp_obj->p_tokens = NAMED_EXP;
         exp_obj->value.named_expr.var_name = (char*)malloc(
	    sizeof(char)*strlen(post_exp[i].value.token->content));
         strcpy(exp_obj->value.named_expr.var_name, post_exp[i].value.token->content);
         expr_stack[++stack_top] = exp_obj;
	 break;
        case PLUS_OP:
          parse_exp_binary(expr_stack,&stack_top,exp_obj,PLUS); break;
	case PRODUCT_OP:
          parse_exp_binary(expr_stack,&stack_top,exp_obj,MULTIPLY); break;
        case DIVIDE_OP:
          parse_exp_binary(expr_stack,&stack_top,exp_obj,DIVIDE); break;
        case MINUS_OP:
          parse_exp_binary(expr_stack,&stack_top,exp_obj,MINUS); break;
	case POWER_OP:
          parse_exp_binary(expr_stack,&stack_top,exp_obj,POWER); break;
	case FLOOR_DIV_OP:
          parse_exp_binary(expr_stack,&stack_top,exp_obj,FLOOR_DIVISION); break;
	case MODULUS_OP:
          parse_exp_binary(expr_stack,&stack_top,exp_obj,MODULO); break;
        case UNARY_FLIP_OP:
          parse_exp_unary(expr_stack,&stack_top,exp_obj,UNARY_FLIP); break;
        case UNARY_PLUS_OP:
          parse_exp_unary(expr_stack,&stack_top,exp_obj,UNARY_PLUS); break;
        case UNARY_MINUS_OP:
          parse_exp_unary(expr_stack,&stack_top,exp_obj,UNARY_MINUS); break;
        case LOG_NOT_OP: 
         parse_exp_unary(expr_stack,&stack_top,exp_obj,LOGICAL_NOT); break;
        case LOG_AND_OP:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,LOGICAL_AND); break;
        case LOG_OR_OP:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,LOGICAL_OR); break;
        case LOG_XOR_OP:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,LOGICAL_XOR); break;
        case BIT_AND_OP:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,BITWISE_AND); break;
        case BIT_OR_OP:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,BITWISE_OR); break;
        case BIT_XOR_OP:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,BITWISE_XOR); break;
        case ARITH_LSHIFT:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,ARITHMATIC_LSHIFT); break;
        case ARITH_RSHIFT:
         parse_exp_binary(expr_stack,&stack_top,exp_obj,ARITHMATIC_RSHIFT); break;
	case GREATER_THAN_OP:
	 parse_exp_binary(expr_stack,&stack_top,exp_obj,GREATER_THAN); break;
	case LESS_THAN_OP:
	 parse_exp_binary(expr_stack,&stack_top,exp_obj,LESS_THAN); break;
	case GREATER_THAN_OR_EQUAL_OP:
	 parse_exp_binary(expr_stack,&stack_top,exp_obj,GREATER_THAN_OR_EQUAL); break;
	case LESS_THAN_OR_EQUAL_OP:
	 parse_exp_binary(expr_stack,&stack_top,exp_obj,LESS_THAN_OR_EQUAL);break;
	case EQUALITY_OP:
	 parse_exp_binary(expr_stack,&stack_top,exp_obj,BANG_EQUALITY); break;
	case NOT_EQUAL_OP:
	 parse_exp_binary(expr_stack,&stack_top,exp_obj,NOT_EQUALITY); break;
      }
    } else {
      Expr* expression = post_exp[i].value.parsed_expression;
      expr_stack[++stack_top] = expression;
    }
  }
  Expr* exp_obj = expr_stack[stack_top--];
  free(expr_stack);
  return exp_obj;
}

FunctionCall* parse_function_call(Lexeme** lexeme_list, uint64_t top,uint64_t *index)  {
  FunctionCall* f_call = (FunctionCall*)malloc(sizeof(FunctionCall));
  f_call->function_name.var_name = malloc(sizeof(char)*strlen(lexeme_list[(*index)]->content));
  strcpy(f_call->function_name.var_name, lexeme_list[(*index)++]->content);
  f_call->arg_length = 0;
  uint64_t arg_top = -1;
  f_call->arg_list = (ArgumentObj**)malloc(sizeof(ArgumentObj*)*1);
  (*index)++;
  bool exit = false;
  while (true)  {
    switch(lexeme_list[(*index)]->lexeme_type)   {
      case RIGHT_BRACE:
	exit = true;
	(*index)++;
	break;
      case COMMA:
	(*index)++;
	break;
      default:
       Expr* _expr = parse_expression(lexeme_list, top, &(*index));
       if (lexeme_list[(*index)]->lexeme_type == SEMICOLON) { 
	  (*index)--;
       }
       f_call->arg_length++;
       arg_top++;
       if (arg_top > 0)  {
         ArgumentObj** arg_obj = realloc(f_call->arg_list,sizeof(ArgumentObj*)*arg_top+1);
	 f_call->arg_list = arg_obj;
       }
       f_call->arg_list[arg_top] = create_arg_object(_expr->p_tokens);
       f_call->arg_list[arg_top]->expression = _expr;
    }
    if (exit) break;
  }
  return f_call;
}

UntilLoop* parse_until_loopstatement(Lexeme** lexeme_list,uint64_t top,uint64_t* index) {
  UntilLoop* until_loop = (UntilLoop*)malloc(sizeof(UntilLoop));
  (*index)++;
  until_loop->condition = parse_expression(lexeme_list,top,&(*index));
  if (lexeme_list[*index]->lexeme_type == RESERVED_WORD)  {
    if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DO) != 0)  {
      fprintf(stderr, "ParserError: missing token 'do' after expression until\n");
      exit(-1);
    }
  }
  (*index)++;
  while (true)  {
    switch (lexeme_list[*index]->lexeme_type) {
      default:
	if (lexeme_list[*index]->lexeme_type == RESERVED_WORD) {
	  if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DONE) == 0) {
	    (*index)++;
	    return until_loop;
	  }
	}
        until_loop->body = body_parser(lexeme_list,top,&(*index),true);
        break;
      case LINE_END:
	(*index)++;
	break;
    }
  }
}

IfStatement* parse_if_statement(Lexeme** lexeme_list, uint64_t top,uint64_t* index)  {
  // parsing if statements!
  IfStatement* if_statement = (IfStatement*)malloc(sizeof(IfStatement));
  (*index)++;
  if_statement->condition = parse_expression(lexeme_list,top,&(*index));
  if (lexeme_list[(*index)]->lexeme_type == RESERVED_WORD) {
    if (strcmp(lexeme_list[(*index)]->content, RESERVED_WORD_DO) != 0)  {
      fprintf(stderr, "ParserError: missing token 'do' after expression if\n");
      exit(-1);
    }
  }
  (*index)++;
  bool loop_interrupt = false;
  while (true)  {
    switch (lexeme_list[*index]->lexeme_type) {
      case RESERVED_WORD:
	if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DONE) == 0) {
	  (*index)++;
	  return if_statement;
	}
        if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_IF) == 0)  {
	  if_statement->body = body_parser(lexeme_list,top,&(*index),true);
	}
        loop_interrupt = (strcmp(lexeme_list[(*index)]->content,RESERVED_WORD_ELSE) == 0)?true:false;
        break;
      case LINE_END:
	(*index)++;
	break;
      default:
        if_statement->body = body_parser(lexeme_list,top,&(*index),true);
        loop_interrupt = (*index >= top)?true:false;
    }
    if (loop_interrupt) break;
  }
  if (lexeme_list[*index]->lexeme_type == RESERVED_WORD)  {
    if (strcmp(lexeme_list[*index]->content,RESERVED_WORD_ELSE) == 0)  {
      (*index)++;
      if_statement->else_body = body_parser(lexeme_list, top,&(*index), true);
      if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DONE) != 0)  {
	fprintf(stderr, "ParserError: Missing token 'done' after else block!");
	exit(-1);
      }
    }
  }
  return if_statement;
}

WhenStatement* parse_when_stment(Lexeme** lexeme_list,uint64_t top,uint64_t* index){
  WhenStatement* when_st = (WhenStatement*)malloc(sizeof(WhenStatement));
  when_st->no_of_cases = 0;
  when_st->cases = (MatchCase**)malloc(sizeof(MatchCase*));
  int64_t arg_top = -1;
  (*index)++;
  when_st->pattern = parse_expression(lexeme_list,top,&(*index));
  if (lexeme_list[*index]->lexeme_type == RESERVED_WORD)  {
    if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DO) == 0) {
      (*index)++;
      bool exit = false;
      while (!exit)   {
        switch(lexeme_list[*index]->lexeme_type)  {
	  case LINE_END:
	   (*index)++;
	   break;
	  case RESERVED_WORD:
	   exit = (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DONE) == 0);
	   break;
	  default:
	   MatchCase* m_case = malloc(sizeof(MatchCase));
	   m_case->pattern = parse_expression(lexeme_list,top,&(*index));
	   if (lexeme_list[*index]->lexeme_type == MAP_OPERATOR)  {
	    (*index)++;
	   }
	   m_case->body = body_parser(lexeme_list,top,&(*index),true);
	   arg_top++;
	   when_st->no_of_cases++;
	   if (arg_top > 0)  {
	     MatchCase** _newcase = realloc(when_st->cases, sizeof(MatchCase)*arg_top+1);
	     when_st->cases = _newcase;
	   }
	   when_st->cases[arg_top] = m_case;
	   (*index)++;
	}	
      }
    }
  }
  return when_st;
}

FunctionDef* parse_function_definition(Lexeme** lexeme_list,uint64_t top,uint64_t* index) {
  FunctionDef* fdef = (FunctionDef*)malloc(sizeof(FunctionDef));
  fdef->arg_length = 0;
  int64_t arg_top = -1;
  (*index)++;
  if (lexeme_list[*index]->lexeme_type != NAMED_LEXEME)  {
    fprintf(stderr, "ParserError: Function Name missing!\n");
    exit(-1);
  }
  fdef->function_name.var_name = malloc(sizeof(char)*strlen(lexeme_list[*index]->content));
  strcpy(fdef->function_name.var_name, lexeme_list[*index]->content);
  (*index)++;
  if (lexeme_list[*index]->lexeme_type != LEFT_BRACE)  {
    fprintf(stderr, "ParserError: Missing left parenthesis!\n");
    exit(-1);
  }
  (*index)++;
  fdef->arg_list = (NamedExpr**)malloc(sizeof(NamedExpr*)*1);
  while (true)  {
    if (lexeme_list[*index]->lexeme_type == NAMED_LEXEME)  {
      NamedExpr* named_exp = malloc(sizeof(NamedExpr));
      named_exp->var_name = (char*)malloc(sizeof(char)*strlen(lexeme_list[*index]->content));
      strcpy(named_exp->var_name, lexeme_list[*index]->content);
      arg_top++;
      if (arg_top > 0) {
	NamedExpr** named_exparr = realloc(fdef->arg_list,sizeof(NamedExpr*)*arg_top+1);
	fdef->arg_list = named_exparr;
      }
      fdef->arg_list[arg_top] = named_exp;
      fdef->arg_length++;
      (*index)++;
    } 
    if (lexeme_list[*index]->lexeme_type == COMMA) { 
      (*index)++;
    } else if (lexeme_list[*index]->lexeme_type == RIGHT_BRACE) { 
      (*index)++;
      break;
    } else {
      fprintf(stderr, "ParserError: invalid token found in function argument list!\n");
      exit(-1);
    }
  }
  if (lexeme_list[*index]->lexeme_type == RESERVED_WORD) {
    if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DO) == 0) {
      (*index)++;
      fdef->body = body_parser(lexeme_list,top,&(*index),true);
    }
  }
  return fdef;
}

EnumDef* parse_enum_definition(Lexeme** lexeme_list, uint64_t top, uint64_t *index) {
  (*index)++;
  EnumDef* enum_def = (EnumDef*)malloc(sizeof(EnumDef));
  enum_def->no_of_constants = 0;
  uint64_t arr_top = -1;
  enum_def->constants = (NamedExpr**)malloc(sizeof(NamedExpr*)*1);
  enum_def->values = (Expr**)malloc(sizeof(Expr*)*1);
  if (lexeme_list[*index]->lexeme_type != NAMED_LEXEME)  {
    fprintf(stderr, "ParserError: Missing token 'enum {Enum_name}' after enum declaration!");
    exit(-1);
  }
  if (!isupper(lexeme_list[*index]->content[0]))  {
    fprintf(stderr, "ParserError: enum name must be an upper case alphabet!");
    exit(-1);
  }
  (*index)++;
  if (lexeme_list[(*index)]->lexeme_type == RESERVED_WORD) {
    if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DO) == 0) {
      (*index)++;
      while (true)  {
	switch(lexeme_list[*index]->lexeme_type)  {
	  case LINE_END:
	    (*index)++;
	    break;
	  case COMMA:
	    (*index)++;
	    break;
	  case RESERVED_WORD:
	    if (strcmp(lexeme_list[*index]->content, RESERVED_WORD_DONE) == 0)  {
	      (*index)++;
	      return enum_def;
	    }
	    break;
	  case NAMED_LEXEME:
	    NamedExpr* name_exp = (NamedExpr*)malloc(sizeof(NamedExpr));
	    name_exp->var_name = malloc(strlen(lexeme_list[*index]->content)*sizeof(char));
	    strcpy(name_exp->var_name, lexeme_list[*index]->content);
	    arr_top++;
	    if (arr_top > 0)  {
	       NamedExpr** tmp1 = realloc(enum_def->constants,sizeof(NamedExpr*)*arr_top+1);
	       Expr** tmp2 = realloc(enum_def->values,sizeof(Expr*)*arr_top+1);
	       enum_def->constants = tmp1;
	       enum_def->values = tmp2;
	    }
	    enum_def->constants[arr_top] = name_exp;
	    enum_def->no_of_constants++;
	    if (lexeme_list[(*index) + 1]->lexeme_type == COMMA)  {
	      enum_def->values[arr_top] = NULL;
	    } else if (lexeme_list[(*index) + 1]->lexeme_type == ASSIGNMENT_OP)  {
	      (*index) += 2;
	      if (lexeme_list[*index]->lexeme_type == DYNAMIC_LIST_LEFT_BRACE)  {
		fprintf(stderr, "ParserError: invalid declaration of mutable object in enum\n");
		exit(-1);
	      }
	      Expr* res_exp = parse_expression(lexeme_list,top,&(*index));
	      enum_def->values[arr_top] = res_exp;
	    } 
	    (*index)++;
	    break;
	}
      }
    }
    fprintf(stderr, "ParserError: missing token after enum declaration!");
    exit(-1);
  } 
  fprintf(stderr, "P");
  exit(-1);
}

VariableDec* parse_variable_dec(Lexeme** lexeme_list,uint64_t top,uint64_t *index,bool is_assignment_exp)  {
   *(index) = (is_assignment_exp)?(*index)+1:(*index);
   NamedExpr* named_expr = create_named_expr(lexeme_list[(*index)]->content);
   VariableDec* var_dec = create_variable_dec();
   var_dec->target = named_expr;
   (*index) += 1;
   if (lexeme_list[(*index)++]->lexeme_type == ASSIGNMENT_OP)   {
     Expr* parsed = parse_expression(lexeme_list, top, &(*index));
     var_dec->value = parsed;
     return var_dec;
   }
   fprintf(stderr,"Parser ERROR: missing token after variable naming!\n");
   exit(-1);
}

ProgramBody* body_parser(Lexeme** lexeme_list,uint64_t top,uint64_t *offset,bool inside_block){
  // bool inside_block (indicates whether )
  ProgramBody* prog_body = malloc(sizeof(ProgramBody));
  prog_body->statements = (Statement**)malloc(sizeof(Statement*)*top);
  prog_body->length = 0;
  int64_t m_top = -1;
  uint64_t i = *offset;
  for (; i < top; i++)  {
    if (lexeme_list[i]->lexeme_type == RESERVED_WORD) {
      if (strcmp(lexeme_list[i]->content, RESERVED_WORD_VAR) == 0) {
       VariableDec* var_dec = parse_variable_dec(lexeme_list, top, &i, true);
       prog_body->statements[++m_top] = (Statement*)malloc(sizeof(Statement));
       prog_body->statements[m_top]->expression_type = VARIABLE_DEC;
       prog_body->statements[m_top]->value.var_declaration = var_dec;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_CONST) == 0)  {
       VariableDec* const_dec = parse_variable_dec(lexeme_list, top, &i, true);
       const_dec->is_constant = true;
       prog_body->statements[++m_top] = (Statement*)malloc(sizeof(Statement));
       prog_body->statements[m_top]->expression_type = CONSTANT_DEC;  // CONSTANT declaration
       prog_body->statements[m_top]->value.var_declaration = const_dec;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_IF) == 0)  {
        IfStatement* if_statement = parse_if_statement(lexeme_list,top,&i);
	prog_body->statements[++m_top] = (Statement*)malloc(sizeof(Statement));
	prog_body->statements[m_top]->expression_type = IF_CONDITIONAL;
	prog_body->statements[m_top]->value.if_statement = if_statement;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_ELSE) == 0)  {
        if (inside_block) break;
	else {
	  fprintf(stderr, "ParserError: if-block must be used before 'else'\n");
	  exit(1);
	}
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_FUNCDEF) == 0)  {
	FunctionDef* fdef = parse_function_definition(lexeme_list,top,&i);
	prog_body->statements[++m_top] = malloc(sizeof(Statement));
	prog_body->statements[m_top]->expression_type = FUNCTION_DEFINITION;
	prog_body->statements[m_top]->value.func_def = fdef;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_ENUM) == 0)  {
	EnumDef* edef = parse_enum_definition(lexeme_list,top,&i);
	prog_body->statements[++m_top] = (Statement*)malloc(sizeof(Statement));
	prog_body->statements[m_top]->expression_type = ENUM_DEFINITION;
	prog_body->statements[m_top]->value.enum_def = edef;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_DONE) == 0)  {
	break;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_UNTIL) == 0) {
	UntilLoop* loop = parse_until_loopstatement(lexeme_list,top,&i);
	prog_body->statements[++m_top] = malloc(sizeof(Statement));
	prog_body->statements[m_top]->expression_type = UNTIL_LOOP;
	prog_body->statements[m_top]->value.until_loop = loop;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_WHEN) == 0)  {
	WhenStatement* when_st = parse_when_stment(lexeme_list,top,&i);
	prog_body->statements[++m_top] = malloc(sizeof(Statement));
	prog_body->statements[m_top]->expression_type = WHEN_STATEMENT;
	prog_body->statements[m_top]->value.when_statement = when_st;
      }
    } else if (lexeme_list[i]->lexeme_type == NAMED_LEXEME)  {
     if (i+1 < top)  {
       if (lexeme_list[i+1]->lexeme_type == ASSIGNMENT_OP)  {
	// create variable declaration instance, then !
        VariableDec* var_dec = parse_variable_dec(lexeme_list, top, &i, false);
        AssignmentExpr* assignment_exp = create_assignment_expr();
	assignment_exp->target = create_named_expr(var_dec->target->var_name);
	assignment_exp->value = (Expr*)malloc(sizeof(Expr));
	assignment_exp->value->p_tokens = var_dec->value->p_tokens;
        assignment_exp->value->value = var_dec->value->value;
	free(var_dec);
        prog_body->statements[++m_top] = (Statement*)malloc(sizeof(Statement));
        prog_body->statements[m_top]->expression_type = ASSIGNMENT_EXP;
        prog_body->statements[m_top]->value.assignment_exp = assignment_exp;
       }
       else if (lexeme_list[i+1]->lexeme_type == LEFT_BRACE)  {
	 FunctionCall* f_call = parse_function_call(lexeme_list,top,&i);
	 Expr* exp_obj = malloc(sizeof(Expr));
	 exp_obj->p_tokens = FUNCTION_CALL;
	 exp_obj->value.function_call = f_call;
	 prog_body->statements[++m_top] = (Statement*)malloc(sizeof(Statement));
	 prog_body->statements[m_top]->expression_type = EXPRESSION;
	 prog_body->statements[m_top]->value.expression = exp_obj;
       }
     }
   } else if (lexeme_list[i]->lexeme_type == RETURN_OPERATOR)  {
     ReturnObj* return_obj = malloc(sizeof(ReturnObj));
     if (lexeme_list[i + 1]->lexeme_type == SEMICOLON )  {
       i++;
       return_obj->expression = NULL;
     } else if (lexeme_list[i + 1]->lexeme_type == LINE_END)  {
       fprintf(stderr, "ParserError: missing token after return operator!");
       exit(-1);
     } else {
	i++;
	return_obj->expression = parse_expression(lexeme_list,top,&i);
	prog_body->statements[++m_top] = malloc(sizeof(Statement));
	prog_body->statements[m_top]->expression_type = RETURN_EXPRESSION;
	prog_body->statements[m_top]->value.return_obj = return_obj;
     }
   } else if (lexeme_list[i]->lexeme_type == SEMICOLON)  {
     if (lexeme_list[i - 1]->lexeme_type == SEMICOLON)  
	break;
   }
  }
  Statement** new_state = realloc(prog_body->statements, sizeof(Statement*)*(m_top+1));
  prog_body->statements = new_state;
  prog_body->length = m_top+1;
  *offset = i;
  return prog_body;
}

ProgramBody* parse(Lexeme** lexeme_list, uint64_t top)  {
  uint64_t offset = 0;
  ProgramBody* pb = body_parser(lexeme_list, top, &offset, false);
  return pb;
}

