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
  lex_obj->content = (char*)malloc(sizeof(char*));
  return lex_obj;
}

static bool is_a_reserved_word(char* word)  {
  if (strcmp(word, RESERVED_WORD_VAR) == 0)  return true;
  else if (strcmp(word, RESERVED_WORD_CONST) == 0) return true;
  else if (strcmp(word, RESERVED_WORD_TRUE) == 0) return true;
  else if (strcmp(word, RESERVED_WORD_FALSE) == 0) return true;
  else if (strcmp(word, RESERVED_WORD_NULL) == 0) return true;
  return false;
}

void tokenizer(Lexeme** lexeme_list, char* file_content, uint64_t *top)   {
  for (int i = 0; i < strlen(file_content); i++)   {
    Lexeme* created_lex = create_lexeme();
    if (isalpha(file_content[i]) != 0)  {
      char* name_expression = (char*)malloc(sizeof(char*));
      while (isalpha(file_content[i]) != 0 || file_content[i] != ' ')  {
	char next[] = {file_content[i++], '\0'};
	strcat(name_expression, (char*)next);
      }
      if (is_a_reserved_word(name_expression) == true)  {
	created_lex->lexeme_type = RESERVED_WORD;
	strcat(created_lex->content, name_expression);
	printf("RESERVED word %s\n", name_expression);
      } 
      else if (strcmp(name_expression, NAMED_OPERATOR_MODULO) == 0)  {
        created_lex->lexeme_type == MODULUS_OP;
	strcat(created_lex->content, name_expression);
	printf("MODULO: %s\n", name_expression);
      }
      else {
	created_lex->lexeme_type = NAMED_LEXEME;
	strcat(created_lex->content, name_expression);
	printf("NAME %s\n", name_expression);
      }
      i--;
      lexeme_list[(*top)++] = created_lex;
    }
    else if (isdigit(file_content[i]) != 0)  {
     char* num_expression = (char*)malloc(sizeof(char*));
     while (isdigit(file_content[i]) != 0 || file_content[i] == '.' || file_content[i] == 'x' || file_content[i] == 'o')  {
       char next[] = {file_content[i++], '\0'};
       strcat(num_expression, (char*)next);
     }
     i--;
     printf("NUM %s\n", num_expression);
     created_lex->lexeme_type = NUMBER;
     strcat(created_lex->content, num_expression);
     lexeme_list[(*top)++] = created_lex;
    }
    else {
      switch(file_content[i])   {
	case '<':
	   if (file_content[i + 1] == '-')  {
	     created_lex->lexeme_type = ASSIGNMENT_OP;
	     created_lex->content = NULL;
	     printf("ASSIGNMENT_OP\n");
	     i++;
	     lexeme_list[(*top)++] = created_lex;
	   }
	   break;
	case '^':
	   created_lex->lexeme_type = POWER_OP;
	   created_lex->content = NULL;
	   printf("POWER_OP\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '+':
	   created_lex->lexeme_type = PLUS_OP;
	   created_lex->content = NULL;
	   printf("PLUS_OP\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '~':
	   created_lex->lexeme_type = UNARY_FLIP_OP;
	   created_lex->content = NULL;
	   printf("UNARY_FLIP\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '-':
	   created_lex->lexeme_type = MINUS_OP;
	   created_lex->content = NULL;
	   printf("MINUS_OP\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '(':
	   created_lex->lexeme_type = LEFT_BRACE;
	   created_lex->content =  NULL;
	   printf("LEFT_BRACE\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '*':
	   printf("PRODUCT\n");
	   created_lex->lexeme_type = PRODUCT_OP;
	   created_lex->content = NULL;
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case ')':
	   created_lex->lexeme_type = RIGHT_BRACE;
	   created_lex->content = NULL;
	   printf("RIGHT BRACE\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '/':
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
           char lit_string[] = "\"";
	   i++;
	   while (file_content[i] != '"')  {
             char next[] = {file_content[i], '\0'};
	     strcat(lit_string, (char*)next);
	     i++;
	   }
	   created_lex->lexeme_type = STRING_LITERAL;
	   created_lex->content = (char*)malloc(sizeof(char)*strlen(lit_string));
	   strcpy(created_lex->content, lit_string);
	   lexeme_list[(*top)++] = created_lex;
	   i++;
	   printf("STRING LITERAL %s\n", lit_string);
	   break;
	case ';':
	   created_lex->lexeme_type = SEMICOLON;
	   created_lex->content = NULL;
	   lexeme_list[(*top)++] = created_lex;
	   printf("SEMICOLON\n");
	   break;
	case '\n':
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


