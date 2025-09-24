#ifndef TRANSPILER_H
#define TRANSPILER_H

#include "parser/parser.h"
#include <llvm-c/Core.h>

LLVMModuleRef ast_llvm_emitter(ProgramBody* program_ast,char* file_name);

#endif

