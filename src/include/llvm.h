#ifndef LLVM_H
#define LLVM_H

#include <llvm-c/Types.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Support.h>
#include "uthash.h"
#include "ast.h"

extern LLVMContextRef TheContext;
extern LLVMModuleRef TheModule;
extern LLVMBuilderRef Builder;

typedef struct VarBinding {
    const char *name;
    LLVMValueRef value;
    UT_hash_handle hh;
} VarBinding;

void compile_root(void);
void print_llvm_ir(void);
void free_variables(void);
void init_llvm_codegen(void);
LLVMValueRef llvm_eval_ast(ASTNode *node);
LLVMValueRef get_variable(const char *name);
void write_llvm_ir_to_file(const char *filename);
void insert_variable(const char *name, LLVMValueRef value);
LLVMValueRef create_printf_function_type(LLVMTypeRef *out_type);

#endif // LLVM_H
