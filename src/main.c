#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "parser/parser.h"
#include "tokenizer/tokenizer.h"
#include <regex.h>

char* read_file_content(char* file)  {
  regex_t my_reg;
  int value = regcomp(&my_reg, "\\.duck$", 0);
  value = regexec(&my_reg, file, 0, NULL, 0);
  if (value == REG_NOMATCH)  {
    fprintf(stderr, "ERROR: invalid file provided!\n");
    exit(-1);
  }
  FILE* f1 = fopen(file, "r");
  if (f1 == NULL)  {
    return NULL;
  }
  char *buffer = (char*)malloc(sizeof(char*));
  char ch;
  do {
    ch = fgetc(f1);
    char next[] = {ch, '\0'};
    strcat(buffer, (char*)next);
  } while (ch != EOF);
  return buffer;
}

int main(int argc, char* argv[])   {
  if (argc < 2) {
   fprintf(stderr, "ERROR: Invalid no arguments provided!\n");
   exit(-1);
  }
  else if (strcmp(argv[1], "tokenize") == 0)  {
    if (argc < 3) {
      fprintf(stderr, "ERROR: No file provided\n");
      exit(-1);
    }
    char* file_content = read_file_content(argv[2]);
    uint64_t top = 0;
    Lexeme** lexeme_list = malloc(sizeof(Lexeme*) * strlen(file_content)-2);
    tokenizer(lexeme_list, file_content, &top);
    printf("Parsing phase: \n");
    parse(lexeme_list, top);
  }
  return 0;
}

