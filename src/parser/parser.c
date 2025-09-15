#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "grammar.h"
#include <stdbool.h>
#include <string.h>

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
    case POWER_OP: return 12;
    case UNARY_PLUS_OP: case UNARY_MINUS_OP: 
    case UNARY_FLIP_OP: return 11;
    case PRODUCT_OP : case DIVIDE_OP: 
    case MODULUS_OP: case FLOOR_DIV_OP: return 10;
    case PLUS_OP: case MINUS_OP: return 9;
    case ARITH_LSHIFT: case ARITH_RSHIFT: return 8;
    case BIT_AND_OP: return 7;
    case BIT_XOR_OP: return 6;
    case BIT_OR_OP: return 5;
    case LOG_NOT_OP: return 4;
    case LOG_AND_OP: return 3;
    case LOG_XOR_OP: return 2;
    case LOG_OR_OP: return 1;
    default: return 0;
  } 
}

void parse_exp_binary(Expr** expr_stack, uint64_t* stack_top,Expr* exp_obj,Operators op)  {
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
  } else if (second->p_tokens == UNARY_EXP)  {
   exp_obj->value.binary_exp->left->value.unary_exp = second->value.unary_exp;
  } 
  if (first->p_tokens == _NUMBER || first->p_tokens == STRING) {
   exp_obj->value.binary_exp->right->value.constant_obj = first->value.constant_obj;
  } else if (first->p_tokens == BINARY_EXP)  {
   exp_obj->value.binary_exp->right->value.binary_exp = first->value.binary_exp;
  } else if (first->p_tokens == NAMED_EXP) {
   exp_obj->value.binary_exp->right->value.named_expr = first->value.named_expr;
  } else if (first->p_tokens == UNARY_EXP)  {
   exp_obj->value.binary_exp->right->value.unary_exp = first->value.unary_exp;
  }
  expr_stack[++(*stack_top)] = exp_obj;
}

void parse_exp_unary(Expr** expr_stack, uint64_t* stack_top,Expr* exp_obj,Operators op) {
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

Expr* parse_expression(Lexeme** lexeme_list, uint64_t top, uint64_t* index)  {
  Lexeme** stack = malloc(sizeof(Lexeme*)*top);
  Lexeme** post_exp = malloc(sizeof(Lexeme*)*top);
  uint64_t stack_top = -1, post_top = -1;
  while (true)  {
    if (lexeme_list[*index]->lexeme_type == SEMICOLON || 
	lexeme_list[*index]->lexeme_type == LINE_END)  {
       break;
    }
    else if (lexeme_list[*index]->lexeme_type == NUMBER ||
	lexeme_list[*index]->lexeme_type == STRING_LITERAL || 
	lexeme_list[*index]->lexeme_type == NAMED_LEXEME || 
	lexeme_list[*index]->lexeme_type == RESERVED_WORD)  {
      post_exp[++post_top] = lexeme_list[(*index)++];
    } 
    else if (lexeme_list[(*index)]->lexeme_type == LEFT_BRACE) {
      stack[++stack_top] = lexeme_list[(*index)++];
    } 
    else if (lexeme_list[(*index)]->lexeme_type == RIGHT_BRACE)  {
      while (stack[stack_top]->lexeme_type != LEFT_BRACE)  {
	Lexeme* lex_tok = stack[stack_top--];
        post_exp[++post_top] = lex_tok;
      }
      stack_top--;
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
            first <= operator_precedance(stack[stack_top]->lexeme_type) &&
	    stack[stack_top]->lexeme_type != LEFT_BRACE)   {
            post_exp[++post_top] = stack[stack_top--];
          }
	}
      }
      stack[++stack_top] = lexeme_list[(*index)++];
    }
  }
  while (stack_top != -1)  
    post_exp[++post_top] = stack[stack_top--];
  free(stack);
  Expr** expr_stack = malloc(sizeof(Expr*)*post_top);
  for (uint64_t i = 0; i <= post_top; i++)   {
    Expr* exp_obj = (Expr*)malloc(sizeof(Expr));
    switch(post_exp[i]->lexeme_type)  {
	case NUMBER: case STRING_LITERAL:
         exp_obj->p_tokens = (post_exp[i]->lexeme_type == NUMBER)? _NUMBER:STRING;
         exp_obj->value.constant_obj.value = (char*)malloc(sizeof(char)*strlen(post_exp[i]->content));
         strcpy(exp_obj->value.constant_obj.value, post_exp[i]->content);
         expr_stack[++stack_top] = exp_obj;
	 break;
        case NAMED_LEXEME:
         exp_obj->p_tokens = NAMED_EXP;
         exp_obj->value.named_expr.var_name = (char*)malloc(
	    sizeof(char)*strlen(post_exp[i]->content));
         strcpy(exp_obj->value.named_expr.var_name, post_exp[i]->content);
         expr_stack[++stack_top] = exp_obj;
	 break;
        case PLUS_OP:
          parse_exp_binary(expr_stack, &stack_top, exp_obj, PLUS); break;
	case PRODUCT_OP:
          parse_exp_binary(expr_stack, &stack_top, exp_obj, MULTIPLY); break;
        case DIVIDE_OP:
          parse_exp_binary(expr_stack, &stack_top, exp_obj, DIVIDE); break;
        case MINUS_OP:
          parse_exp_binary(expr_stack, &stack_top, exp_obj, MINUS); break;
	case POWER_OP:
          parse_exp_binary(expr_stack, &stack_top, exp_obj, POWER); break;
	case FLOOR_DIV_OP:
          parse_exp_binary(expr_stack, &stack_top, exp_obj, FLOOR_DIVISION); break;
	case MODULUS_OP:
          parse_exp_binary(expr_stack, &stack_top, exp_obj, MODULO); break;
        case UNARY_FLIP_OP:
          parse_exp_unary(expr_stack, &stack_top, exp_obj, UNARY_FLIP); break;
        case UNARY_PLUS_OP:
          parse_exp_unary(expr_stack, &stack_top, exp_obj, UNARY_PLUS); break;
        case UNARY_MINUS_OP:
          parse_exp_unary(expr_stack, &stack_top, exp_obj, UNARY_MINUS); break;
        case LOG_NOT_OP: 
         parse_exp_unary(expr_stack, &stack_top, exp_obj, LOGICAL_NOT); break;
        case LOG_AND_OP:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, LOGICAL_AND); break;
        case LOG_OR_OP:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, LOGICAL_OR); break;
        case LOG_XOR_OP:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, LOGICAL_XOR); break;
        case BIT_AND_OP:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, BITWISE_AND); break;
        case BIT_OR_OP:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, BITWISE_OR); break;
        case BIT_XOR_OP:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, BITWISE_XOR); break;
        case ARITH_LSHIFT:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, ARITHMATIC_LSHIFT); break;
        case ARITH_RSHIFT:
         parse_exp_binary(expr_stack, &stack_top, exp_obj, ARITHMATIC_RSHIFT); break;
    }
  }
  Expr* exp_obj = expr_stack[stack_top--];
  free(expr_stack);
  return exp_obj;
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

void parse(Lexeme** lexeme_list, uint64_t top)  {
  Instruction** module = (Instruction**)malloc(sizeof(Instruction*)*top);
  uint64_t m_top = -1;
  for (uint64_t i = 0; i < top; i++)  {
    if (lexeme_list[i]->lexeme_type == RESERVED_WORD) {
      if (strcmp(lexeme_list[i]->content, RESERVED_WORD_VAR) == 0) {
       VariableDec* var_dec = parse_variable_dec(lexeme_list, top, &i, true);
       module[++m_top] = (Instruction*)malloc(sizeof(Instruction));
       module[m_top]->expression_type = VARIABLE_DEC;
       module[m_top]->value.var_declaration = var_dec;
      } else if (strcmp(lexeme_list[i]->content, RESERVED_WORD_CONST) == 0)  {
       VariableDec* const_dec = parse_variable_dec(lexeme_list, top, &i, true);
       const_dec->is_constant = true;
       module[++m_top] = (Instruction*)malloc(sizeof(Instruction));
       module[m_top]->expression_type = CONSTANT_DEC;  // CONSTANT declaration
       module[m_top]->value.var_declaration = const_dec;
      } 
    }  
    else if (lexeme_list[i]->lexeme_type == NAMED_LEXEME)  {
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
        module[++m_top] = (Instruction*)malloc(sizeof(Instruction));
        module[m_top]->expression_type = ASSIGNMENT_EXP;
        module[m_top]->value.assignment_exp = assignment_exp;
       }
     }
   }
  }
}

