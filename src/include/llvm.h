#ifndef LLVM_H
#define LLVM_H

#include <llvm-c/Types.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Support.h>
#include "ast.h"

extern LLVMContextRef TheContext;
extern LLVMModuleRef TheModule;
extern LLVMBuilderRef Builder;

void print_llvm_ir(void);
void init_llvm_codegen(void);
LLVMValueRef create_main_function(void);
LLVMValueRef llvm_eval_ast(ASTNode *node);
void write_llvm_ir_to_file(const char *filename);
LLVMValueRef create_printf_function_type(LLVMTypeRef *out_type);

#endif // LLVM_H
