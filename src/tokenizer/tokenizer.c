#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "grammar.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

Lexeme* create_lexeme()  {
  Lexeme* lex_obj = (Lexeme* )malloc(sizeof(Lexeme));
  if (lex_obj == NULL)  {
    fprintf(stderr, "Error: can't create lexeme!");
    return NULL;
  }
  lex_obj->content = (char*)malloc(sizeof(char));
  return lex_obj;
}

static bool is_a_reserved_word(char* word)  {
  if (strcmp(word, RESERVED_WORD_VAR) == 0)  return true;
  else if (strcmp(word, RESERVED_WORD_CONST) == 0) return true;
  else if (strcmp(word, RESERVED_WORD_NULL) == 0) return true;
  return false;
}

static void tokenize_comparision_or_minus_op(Lexeme** list,uint64_t *l_top, char* file_data,
   uint64_t* index_ptr){
  /* tokenizes operators, like (-gt, lt, -geq, -leq, -eq, -neq, -)*/
  Lexeme* created_lex = create_lexeme();
  if (file_data[(*index_ptr) + 1] == 'g')  {
    if (file_data[(*index_ptr) + 2] == 't')  {
       created_lex->lexeme_type = GREATER_THAN_OP;
       printf("GREATER THAN OP!\n");
       created_lex->content = NULL;
       (*index_ptr) += 2;
    } else if (file_data[(*index_ptr) + 2] == 'e')  {
	if (file_data[(*index_ptr) + 3] == 'q')  {
	  created_lex->lexeme_type = GREATER_THAN_OR_EQUAL_OP;  
	  // it greater than or equal to operator
	  printf("GREATER_THAN_OR_EQUAL\n");
	  created_lex->content = NULL;
	  (*index_ptr) += 3;
	}
	else {
	  fprintf(stderr, "LexicalError: Invalid token '%c'\n", file_data[(*index_ptr)+3]);
	  exit(1);
	}
     }
     else {
       fprintf(stderr, "LexicalError: Invalid token '%c'\n", file_data[(*index_ptr) + 2]);
       exit(1);
     }
  } else if (file_data[(*index_ptr) + 1] == 'l')  {
    if (file_data[(*index_ptr) + 2] == 't')  {
       created_lex->lexeme_type = LESS_THAN_OP;
       printf("LESS Than OP\n");
       created_lex->content = NULL;
       (*index_ptr) += 2;
    } else if (file_data[(*index_ptr) + 2] == 'e')  {
	if (file_data[(*index_ptr) + 3] == 'q')  {
	  created_lex->lexeme_type = LESS_THAN_OR_EQUAL_OP;  
	  // it greater than or equal to operator
	  printf("LESS_THAN_OR_EQUAL\n");
	  created_lex->content = NULL;
	  (*index_ptr) += 3;
	}
	else {
	  fprintf(stderr, "LexicalError: Invalid token: '%c'\n", file_data[(*index_ptr)+3]);
	  exit(1);
	}
     }
     else {
       fprintf(stderr, "LexicalError: Invalid token '%c'\n", file_data[(*index_ptr) + 2]);
       exit(1);
     }
  } else if (file_data[(*index_ptr) + 1] == 'e') {
    if (file_data[(*index_ptr) + 2] == 'q')  {
      created_lex->lexeme_type = EQUALITY_OP;
      created_lex->content = NULL;
      printf("EQUALITY_OP\n");
      (*index_ptr) += 2;
    } else {
      fprintf(stderr, "LexicalError: Invalid token '%c'\n", file_data[(*index_ptr) + 2]);
      exit(1);
    }
  } else if (file_data[(*index_ptr) + 1] == 'n')  {
    if (file_data[(*index_ptr) + 2] == 'e')   {
      if (file_data[(*index_ptr) + 3] == 'q')  {
        created_lex->lexeme_type = NOT_EQUAL_OP;
	created_lex->content = NULL;
	printf("NOT_EQUAL_OP\n");
	(*index_ptr) += 3;
      }
    } else {
      fprintf(stderr, "LexicalError: Invalid token '%c'\n", file_data[(*index_ptr) + 2]);
      exit(1);
    }
  }
  else {
      created_lex->lexeme_type = MINUS_OP;
      created_lex->content = NULL;
      printf("MINUS_OP\n");
  }
  list[(*l_top)++] = created_lex;
}

static void tokenize_text(Lexeme** list, uint64_t *l_top,char* file_data,uint64_t* index_ptr)  {
  /* tokenizes keywords, variable names, function-names! */
  Lexeme* created_lex = create_lexeme();
  char *name_expression = (char*)malloc(sizeof(char));
  uint64_t size = 1;
  while (file_data[(*index_ptr)] != ' ' && file_data[(*index_ptr)] != ';')  {
     if (!(file_data[(*index_ptr)] == '_' || isdigit(file_data[(*index_ptr)]) != 0 
       || isalpha(file_data[(*index_ptr)]) != 0)) {
       printf("LexicalError: Invalid token found '%c'\n", file_data[(*index_ptr)]);
       exit(1);
     }
     char next[] = {file_data[(*index_ptr)++], '\0'};
     char* temp = realloc(name_expression, sizeof(char)*size+1);
     if (temp == NULL) {
       fprintf(stderr, "Cannot read buffer!\n");
       exit(-1);
     }
     name_expression = temp;
     strcat(name_expression, (char*)next);
     size++;
  }
  if (is_a_reserved_word(name_expression) == true)  {
    created_lex->lexeme_type = RESERVED_WORD;
    strcat(created_lex->content, name_expression);
    printf("RESERVED word %s\n", name_expression);
  } else if (strcmp(name_expression, NAMED_OPERATOR_MODULO) == 0)  {
     created_lex->lexeme_type = MODULUS_OP;
     free(created_lex->content);
     printf("MODULO: %s\n", name_expression);
  } else if (strcmp(name_expression, NAMED_OPERATOR_AND) == 0)  {
     created_lex->lexeme_type = LOG_AND_OP;
     free(created_lex->content);
     printf("LOGICAL AND: %s\n", name_expression);
  } else if (strcmp(name_expression, NAMED_OPERATOR_OR) == 0) {
     created_lex->lexeme_type = LOG_OR_OP;
     free(created_lex->content);
     printf("LOGICAL OR %s\n", name_expression);
   } else if (strcmp(name_expression, NAMED_OPERATOR_NOT) == 0)  {
     created_lex->lexeme_type = LOG_NOT_OP;
     free(created_lex->content);
     printf("LOGICAL NOT %s\n", name_expression);
   } else if (strcmp(name_expression, NAMED_OPERATOR_XOR) == 0)  {
     created_lex->lexeme_type = LOG_XOR_OP;
     free(created_lex->content);
     printf("LOGICAL XOR %s\n", name_expression);
   } else {
     created_lex->lexeme_type = NAMED_LEXEME;
     created_lex->content = (char*)malloc(sizeof(char)*strlen(name_expression));
     strcat(created_lex->content, name_expression);
     printf("NAME %s\n", name_expression);
   }
   list[(*l_top)++] = created_lex;
}

static void tokenize_numeric_literal(Lexeme** list, uint64_t* l_top, char* file_data,
 uint64_t* index_ptr)  {
  /* tokenizes numbers and floating points! */
  Lexeme* created_lex = create_lexeme();
  char* num_expression = (char*)malloc(sizeof(char));
  uint64_t size = 1;
  while (isdigit(file_data[(*index_ptr)]) != 0 || file_data[(*index_ptr)] == '.')  {
    char next[] = {file_data[(*index_ptr)++], '\0'};
    char* temp = realloc(num_expression, sizeof(char)*size+1);
    if (temp == NULL)  {
      fprintf(stderr, "Cannot read buffer!\n");
      exit(-1);
    }
    num_expression = temp;
    strcat(num_expression, (char*)next);
    size++;
  }
  (*index_ptr)--;
  printf("NUM %s\n", num_expression);
  created_lex->lexeme_type = NUMBER;
  strcat(created_lex->content, num_expression);
  list[(*l_top)++] = created_lex;
}

static void tokenize_string_literal(Lexeme** list, uint64_t* l_top, char* file_data,uint64_t*index_ptr)  {
  /* tokenizes string literals! */
  Lexeme* created_lex = create_lexeme();
  char *lit_string = (char*)malloc(sizeof(char)*2);
  strcpy(lit_string, "\"");
  (*index_ptr)++;

  while (file_data[(*index_ptr)] != '"')  {
   char next[] = {file_data[(*index_ptr)], '\0'};
   strcat(lit_string, (char*)next);
   (*index_ptr)++;
  }
  strcat(lit_string, "\"");
  created_lex->lexeme_type = STRING_LITERAL;
  created_lex->content = (char*)malloc(sizeof(char)*strlen(lit_string));
  strcpy(created_lex->content, lit_string);
  list[(*l_top)++] = created_lex;
  printf("STRING LITERAL %s\n", lit_string);
}

static void tokenize_assignment_and_arithlsh(Lexeme** list, uint64_t* l_top, char* file_data,
 uint64_t* index_ptr)  {
   // tokenize assignment expression and arithmatic left shift (<-, <<)!
   Lexeme* created_lex = create_lexeme(); 
   if (file_data[(*index_ptr) + 1] == '-')  {
     created_lex->lexeme_type = ASSIGNMENT_OP;
     created_lex->content = NULL;
     printf("ASSIGNMENT_OP\n");
     (*index_ptr)++;
     list[(*l_top)++] = created_lex;
   }
   else if (file_data[(*index_ptr) + 1] == '<')  {
     created_lex->lexeme_type = ARITH_LSHIFT;
     created_lex->content = NULL;
     printf("ARITHMATIC LSHIFT\n");
     (*index_ptr)++;
     list[(*l_top)++] = created_lex;
   } else {
     fprintf(stderr, "LexicalError: invalid operator %c\n",file_data[(*index_ptr)+1]);
     exit(1);
   }
}

static void tokenize_lbrace_or_multiline_comment(Lexeme** list,uint64_t* l_top, char* file_data,
 uint64_t*index_ptr)  {
   // tokenize left brace  Multi-line comment is going to be removed!
   if (file_data[(*index_ptr) + 1] == '*')   {
     (*index_ptr) += 2;
     while (true)  {
       if (file_data[(*index_ptr)] == '*' && file_data[(*index_ptr) + 1] == ')') 
	   break;
       (*index_ptr)++;
     }
     (*index_ptr) += 1;
   } else {
     Lexeme* created_lex = create_lexeme();
     created_lex->lexeme_type = LEFT_BRACE;
     created_lex->content =  NULL;
     printf("LEFT_BRACE\n");
     list[(*l_top)++] = created_lex;
  }
}

void tokenizer(Lexeme** lexeme_list, char* file_content, uint64_t *top)   {
  for (uint64_t i = 0; i < strlen(file_content); i++)   {
    if (isalpha(file_content[i]) != 0)  {
      tokenize_text(lexeme_list, &(*top),file_content,&i);
    }
    else if (isdigit(file_content[i]) != 0)  {
     tokenize_numeric_literal(lexeme_list, &(*top),file_content,&i);
    }
    else {
      Lexeme* created_lexeme = NULL;
      switch(file_content[i])   {
	case '<':
	   tokenize_assignment_and_arithlsh(lexeme_list,&(*top),file_content,&i);
	   break;
	case '>':
	   Lexeme* created_lex = create_lexeme();
	   if (file_content[i + 1] == '>')  {
	     created_lex->lexeme_type = ARITH_RSHIFT;
	     created_lex->content = NULL;
	     printf("ARITHMATIC RSHIFT");
	     i++;
	     lexeme_list[(*top)++] = created_lex;
	   } else {
	     fprintf(stderr, "LexicalError: invalid token %c\n", file_content[i+1]);
	     exit(1);
	   }
	   break;
	case '^':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = POWER_OP;
	   created_lex->content = NULL;
	   printf("POWER_OP\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '&':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = BIT_AND_OP;
	   created_lex->content = NULL;
	   printf("BITWISE AND\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '|':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = BIT_OR_OP;
	   created_lex->content = NULL;
	   printf("BITWISE OR\n");
	   lexeme_list[(*top)++] = created_lex;
	   break; 
	case '+':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = PLUS_OP;
	   created_lex->content = NULL;
	   printf("PLUS_OP\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '?':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = BIT_XOR_OP;
	   created_lex->content = NULL;
	   printf("BITWISE XOR\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '~':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = UNARY_FLIP_OP;
	   created_lex->content = NULL;
	   printf("UNARY_FLIP\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '-':
	   tokenize_comparision_or_minus_op(lexeme_list, &(*top), file_content, &i);
	   break;
	case '(':
	   tokenize_lbrace_or_multiline_comment(lexeme_list,&(*top),file_content,&i);
	   break;
	case '$':
	   while (true)  {
             if (file_content[i] == '\n' || file_content[i] == '\0')
		break;
	     i++;
	   }
	   break;
	case '*':
	   created_lex = create_lexeme();
	   printf("PRODUCT\n");
	   created_lex->lexeme_type = PRODUCT_OP;
	   created_lex->content = NULL;
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case ')':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = RIGHT_BRACE;
	   created_lex->content = NULL;
	   printf("RIGHT BRACE\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '/':
	   created_lex = create_lexeme();
	   if (file_content[i + 1] == '/')   {
            created_lex->lexeme_type = FLOOR_DIV_OP;
	    created_lex->content = NULL;
	    lexeme_list[(*top)++] = created_lex;
	    i++;
	    printf("FLOOR DIV\n");
	   } else {
            created_lex->lexeme_type = DIVIDE_OP;
	    created_lex->content = NULL;
	    lexeme_list[(*top)++] = created_lex;
	    printf("DIV\n");
	   }
	   break;
	case '"':
           tokenize_string_literal(lexeme_list,&(*top),file_content,&i);
	   break; 
	case '{':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = DYNAMIC_LIST_LEFT_BRACE;
	   created_lex->content = NULL;
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '}':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = DYNAMIC_LIST_RIGHT_BRACE;
	   created_lex->content = NULL;
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case ';':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = SEMICOLON;
	   created_lex->content = NULL;
	   lexeme_list[(*top)++] = created_lex;
	   printf("SEMICOLON\n");
	   break;
	case '\n':
	   created_lex = create_lexeme();
	   created_lex->lexeme_type = LINE_END;
	   created_lex->content = NULL;
	   lexeme_list[(*top)++] = created_lex;
	   printf("NEW_LINE\n");
	   break;
	case ' ':
	   break;
      }
    }
  }
}


