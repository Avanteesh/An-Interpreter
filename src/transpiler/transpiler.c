#include "parser/parser.h"
#include "transpiler/transpiler.h"
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static LLVMTypeRef i32_type, double_type, i8_ptr_type, union_type;
static LLVMTypeRef printf_arg_type, boxed_struct;
static LLVMTypeRef boxed_ptr_type, printf_type;
static LLVMValueRef printf_func, null_ptr;

static void emit_var_declaration(LLVMContextRef context, LLVMBuilderRef builder, VariableDec* var_dec){
  LLVMValueRef variable = LLVMBuildAlloca(builder,boxed_struct,var_dec->target->var_name);
  switch(var_dec->value->p_tokens)  {
    case _NUMBER:
      LLVMValueRef num = LLVMConstReal(
	LLVMDoubleTypeInContext(context),atof(var_dec->value->value.constant_obj.value)
      );
      LLVMValueRef var_ptr = LLVMBuildStructGEP2(builder,double_type,variable, 0,"var");
      LLVMBuildStore(builder,num,var_ptr);
      break;
  }
}

static void emit_constant_declaration(LLVMModuleRef module, LLVMContextRef context, LLVMBuilderRef builder, VariableDec* var_dec){
  LLVMValueRef global_const = LLVMAddGlobal(module, boxed_struct, var_dec->target->var_name);
  LLVMSetGlobalConstant(global_const, true);
  LLVMSetLinkage(global_const, LLVMInternalLinkage);
  switch (var_dec->value->p_tokens)  {
    case _NUMBER:
      LLVMValueRef tag_const = LLVMConstInt(i32_type,0,false);
      LLVMValueRef double_const = LLVMConstReal(double_type,atof(var_dec->value->value.constant_obj.value));  
      LLVMValueRef union_const = LLVMConstStruct((LLVMValueRef[]){double_const, null_ptr},2,false);
      LLVMValueRef struct_const = LLVMConstStruct((LLVMValueRef[]){tag_const, union_const},2,false);      
      LLVMSetInitializer(global_const, struct_const);
      break;
  }
}

static void emit_function_call(LLVMBuilderRef builder, FunctionCall* f_call)  {
  if (strcmp(f_call->function_name.var_name, "print") == 0) {
    LLVMValueRef v_loaded = LLVMBuildAlloca(
      builder, LLVMDoubleType(), 
      f_call->arg_list[0]->expression->value.named_expr.var_name
    );
    /*
    LLVMValueRef v_loaded = LLVMBuildLoad2(
      builder, LLVMDoubleType(), 
      x_alloca, f_call->arg_list[0]->expression->value.named_expr.var_name
    );*/
    LLVMValueRef fmt_str = LLVMBuildGlobalStringPtr(builder, "%f\n", "fmt");    
    LLVMValueRef args[] = { fmt_str, v_loaded };
    LLVMBuildCall2(builder, printf_type, printf_func, args, 2, "");
  }
}

LLVMModuleRef ast_llvm_emitter(ProgramBody* program_ast, char* file_name){
   LLVMContextRef context = LLVMContextCreate();
   LLVMModuleRef module = LLVMModuleCreateWithNameInContext(file_name,context);
   LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
   double_type = LLVMDoubleTypeInContext(context);
   i32_type = LLVMInt32TypeInContext(context);
   i8_ptr_type = LLVMPointerType(LLVMInt8TypeInContext(context), 0);
   null_ptr = LLVMConstNull(i8_ptr_type);
   union_type = LLVMStructTypeInContext(
     context,(LLVMTypeRef[]){ double_type, i8_ptr_type},2,false
   );
   boxed_struct = LLVMStructTypeInContext(
    context,(LLVMTypeRef[]){i32_type, union_type},2,false
   );
   printf_arg_type = LLVMPointerType(LLVMInt8Type(), 0);
   printf_type = LLVMFunctionType(LLVMInt32Type(), &printf_arg_type, 1,1);
   printf_func = LLVMAddFunction(module, "printf", printf_type);   
   boxed_ptr_type = LLVMPointerType(boxed_struct, 0);
   LLVMTypeRef main_type = LLVMFunctionType(i8_ptr_type, NULL, 0,0);
   LLVMValueRef main_func = LLVMAddFunction(module, "main", main_type);
   LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context,main_func,"entry");
   LLVMPositionBuilderAtEnd(builder, entry);
   for (int i = 0; i < program_ast->length; i++)  {
     Statement* st = program_ast->statements[i];
     switch(st->expression_type)  {
	case VARIABLE_DEC:
	   emit_var_declaration(context,builder,st->value.var_declaration);
	   break;
	case CONSTANT_DEC:
	   emit_constant_declaration(module,context,builder,st->value.var_declaration);
	   break;
	case EXPRESSION:
	   if (st->value.expression->p_tokens == FUNCTION_CALL)  {
	     emit_function_call(builder,st->value.expression->value.function_call);
	   }
	   break;
     }
   }
   LLVMBuildRet(builder, LLVMConstInt(LLVMInt32Type(), 0, 0));
   char* code = LLVMPrintModuleToString(module);
   printf("%s\n", code);
   return module;
}



