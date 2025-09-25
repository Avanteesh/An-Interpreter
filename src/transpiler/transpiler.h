#ifndef TRANSPILER_H
#define TRANSPILER_H

#include "parser/parser.h"
#include <llvm-c/Core.h>
#include <stdint.h>
#include <llvm-c/Analysis.h>

typedef struct {
  LLVMValueRef value_ref;
  char* symbol_name;
} Symbol;

typedef struct {
  uint64_t length;
  Symbol** symbol_table;
} SymbolTable;

LLVMModuleRef ast_llvm_emitter(ProgramBody* program_ast,char* file_name);

#endif

