#include "parser/parser.h"
#include "transpiler/transpiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static char* transpile_function_definition(TranspiledCode* cod, FunctionDef* fdef)  {
  return NULL; 
}

TranspiledCode* ast_traverser_and_transpiler(ProgramBody* program_ast) {
   TranspiledCode* code = malloc(sizeof(TranspiledCode));
   code->buffer_size = 0;
   code->llvm_ir_code = malloc(sizeof(char)*1);
   for (int i = 0; i < program_ast->length; i++)  {
     switch(program_ast->statements[i]->expression_type)  {
	case FUNCTION_DEFINITION:
	  //...
     }
   }
   return code;
}



