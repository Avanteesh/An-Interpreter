#ifndef TRANSPILER_H
#define TRANSPILER_H

#include "parser/parser.h"

typedef struct {
  char* llvm_ir_code;
  uint64_t buffer_size;
} TranspiledCode;

TranspiledCode* ast_traverser_and_transpiler(ProgramBody* program_ast);
#endif

