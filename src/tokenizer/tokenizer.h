#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "grammar.h"
#include <stdint.h>

typedef struct {
  Tokens lexeme_type;
  char* content;
} Lexeme;

Lexeme* create_lexeme();
void tokenizer(Lexeme** lexem_list, char* file_content, uint64_t *top);

#endif
