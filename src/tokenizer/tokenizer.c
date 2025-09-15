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

void tokenizer(Lexeme** lexeme_list, char* file_content, uint64_t *top)   {
  for (int i = 0; i < strlen(file_content); i++)   {
    Lexeme* created_lex = create_lexeme();
    if (isalpha(file_content[i]) != 0)  {
      char *name_expression = (char*)malloc(sizeof(char));
      uint64_t size = 1;
      while (file_content[i] != ' ' && file_content[i] != ';')  {
	if (!(file_content[i] == '_' || isdigit(file_content[i]) != 0 
	 || isalpha(file_content[i]) != 0)) {
          printf("LexicalError: Invalid token found '%c'\n", file_content[i]);
          exit(1);
	}
	char next[] = {file_content[i++], '\0'};
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
      } 
      else if (strcmp(name_expression, NAMED_OPERATOR_MODULO) == 0)  {
        created_lex->lexeme_type = MODULUS_OP;
	free(created_lex->content);
	printf("MODULO: %s\n", name_expression);
      }
      else if (strcmp(name_expression, NAMED_OPERATOR_AND) == 0)  {
        created_lex->lexeme_type = LOG_AND_OP;
	free(created_lex->content);
	printf("LOGICAL AND: %s\n", name_expression);
      } 
      else if (strcmp(name_expression, NAMED_OPERATOR_OR) == 0) {
        created_lex->lexeme_type = LOG_OR_OP;
	free(created_lex->content);
	printf("LOGICAL OR %s\n", name_expression);
      } 
      else if (strcmp(name_expression, NAMED_OPERATOR_NOT) == 0)  {
        created_lex->lexeme_type = LOG_NOT_OP;
	free(created_lex->content);
	printf("LOGICAL NOT %s\n", name_expression);
      }
      else if (strcmp(name_expression, NAMED_OPERATOR_XOR) == 0)  {
        created_lex->lexeme_type = LOG_XOR_OP;
	free(created_lex->content);
	printf("LOGICAL XOR %s\n", name_expression);
      }
      else {
	created_lex->lexeme_type = NAMED_LEXEME;
	created_lex->content = (char*)malloc(sizeof(char)*strlen(name_expression));
	strcat(created_lex->content, name_expression);
	printf("NAME %s\n", name_expression);
      }
      lexeme_list[(*top)++] = created_lex;
    }
    else if (isdigit(file_content[i]) != 0)  {
     char* num_expression = (char*)malloc(sizeof(char));
     uint64_t size = 1;
     while (isdigit(file_content[i]) != 0 || file_content[i] == '.' || file_content[i] == 'x')  {
       char next[] = {file_content[i++], '\0'};
       char* temp = realloc(num_expression, sizeof(char)*size+1);
       if (temp == NULL)  {
	 fprintf(stderr, "Cannot read buffer!\n");
	 exit(-1);
       }
       num_expression = temp;
       strcat(num_expression, (char*)next);
       size++;
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
	   else if (file_content[i + 1] == '<')  {
	     created_lex->lexeme_type = ARITH_LSHIFT;
	     created_lex->content = NULL;
	     printf("ARITHMATIC LSHIFT\n");
	     i++;
	     lexeme_list[(*top)++] = created_lex;
	   }
	   else if (file_content[i + 1] == '>')   {
	     created_lex->lexeme_type = BIT_XOR_OP;
	     created_lex->content = NULL;
	     printf("BITWISE XOR\n");
	     i++;
	     lexeme_list[(*top)++] = created_lex;
	   }
	   break;
	case '>':
	   if (file_content[i + 1] == '>')  {
	     created_lex->lexeme_type = ARITH_RSHIFT;
	     created_lex->content = NULL;
	     printf("ARITHMATIC RSHIFT");
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
	case '&':
	   created_lex->lexeme_type = BIT_AND_OP;
	   created_lex->content = NULL;
	   printf("BITWISE AND\n");
	   lexeme_list[(*top)++] = created_lex;
	   break;
	case '|':
	   created_lex->lexeme_type = BIT_OR_OP;
	   created_lex->content = NULL;
	   printf("BITWISE OR\n");
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
	   if (file_content[i + 1] == '*')   {
	     i += 2;
	     while (true)  {
	       if (file_content[i] == '*' && file_content[i + 1] == ')') 
		 break;
	       i++;
	     }
	     i += 1;
	   } 
	   else {
	     created_lex->lexeme_type = LEFT_BRACE;
	     created_lex->content =  NULL;
	     printf("LEFT_BRACE\n");
	     lexeme_list[(*top)++] = created_lex;
	   }
	   break;
	case '$':
	   while (true)  {
             if (file_content[i] == '\n' || file_content[i] == '\0')
		break;
	     i++;
	   }
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
           char *lit_string = (char*)malloc(sizeof(char)*2);
	   strcpy(lit_string, "\"");
	   i++;
	   while (file_content[i] != '"')  {
             char next[] = {file_content[i], '\0'};
	     strcat(lit_string, (char*)next);
	     i++;
	   }
	   strcat(lit_string, "\"");
	   created_lex->lexeme_type = STRING_LITERAL;
	   created_lex->content = (char*)malloc(sizeof(char)*strlen(lit_string));
	   strcpy(created_lex->content, lit_string);
	   lexeme_list[(*top)++] = created_lex;
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


