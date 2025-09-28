#ifndef TRANSPILER_H
#define TRANSPILER_H

#include "parser/parser.h"
#include <llvm-c/Core.h>
#include <stdint.h>
#include <llvm-c/Analysis.h>

typedef enum {
  GLOBAL, LOCAL
} Scopes;

typedef enum {
  VARIABLE,
  CONSTANT,
  FUNCTION
} ParametricType;

typedef struct {
 char* name;
 Scopes scope;
 ParametricType type;
 LLVMValueRef value_ref;
} Symbol;

LLVMModuleRef ast_llvm_emitter(ProgramBody* program_ast,char* file_name);

#endif

