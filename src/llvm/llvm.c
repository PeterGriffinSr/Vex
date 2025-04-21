#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "llvm.h"
#include "ast.h"

extern ASTNode *root;

LLVMContextRef TheContext;
LLVMModuleRef TheModule;
LLVMBuilderRef Builder;
static LLVMValueRef printf_func = NULL;

LLVMValueRef create_printf_function_type(LLVMTypeRef *out_type) {
    if (printf_func != NULL) {
        if (out_type) {
            LLVMTypeRef printf_arg_types[] = {
                LLVMPointerType(LLVMInt8TypeInContext(TheContext), 0)
            };
            *out_type = LLVMFunctionType(
                LLVMInt32TypeInContext(TheContext), printf_arg_types, 1, true
            );
        }
        return printf_func;
    }

    LLVMTypeRef printf_arg_types[] = {
        LLVMPointerType(LLVMInt8TypeInContext(TheContext), 0)
    };
    LLVMTypeRef printf_type = LLVMFunctionType(
        LLVMInt32TypeInContext(TheContext), printf_arg_types, 1, true
    );
    printf_func = LLVMAddFunction(TheModule, "printf", printf_type);

    if (out_type) *out_type = printf_type;
    return printf_func;
}


void init_llvm_codegen(void) {
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    TheContext = LLVMContextCreate();
    TheModule = LLVMModuleCreateWithNameInContext("vex_module", TheContext);
    Builder = LLVMCreateBuilderInContext(TheContext);
}

LLVMValueRef llvm_eval_ast(ASTNode *node) {
    switch (node->type) {
        case NodeIntLit: {
            return LLVMConstInt(LLVMInt64TypeInContext(TheContext), (long long unsigned int)node->intval, 0);
        }

        case NodeFloatLit: {
            return LLVMConstReal(LLVMDoubleTypeInContext(TheContext), node->floatval);
        }

        case NodeCharLit: {
            return LLVMConstInt(LLVMInt8TypeInContext(TheContext), (unsigned char)node->charval, 0);
        }

        case NodeStringLit: {
            return LLVMBuildGlobalStringPtr(Builder, node->strval, "strtmp");
        }

        case NodeBinaryExpr: {
            LLVMValueRef left = llvm_eval_ast(node->binary_expr.left);
            LLVMValueRef right = llvm_eval_ast(node->binary_expr.right);
            const char *op = node->binary_expr.op;

            if (left && right) {
                if (strcmp(op, "+") == 0)
                    return LLVMBuildAdd(Builder, left, right, "addtmp");
                else if (strcmp(op, "-") == 0)
                    return LLVMBuildSub(Builder, left, right, "subtmp");
                else if (strcmp(op, "*") == 0)
                    return LLVMBuildMul(Builder, left, right, "multmp");
                else if (strcmp(op, "/") == 0)
                    return LLVMBuildSDiv(Builder, left, right, "divtmp");
                else if (strcmp(op, "+.") == 0)
                    return LLVMBuildFAdd(Builder, left, right, "faddtmp");
                else if (strcmp(op, "-.") == 0)
                    return LLVMBuildFSub(Builder, left, right, "fsubtmp");
                else if (strcmp(op, "*.") == 0)
                    return LLVMBuildFMul(Builder, left, right, "fmultmp");
                else if (strcmp(op, "/.") == 0)
                    return LLVMBuildFDiv(Builder, left, right, "fdivtmp");
            }

            fprintf(stderr, "LLVM error: unsupported binary operator '%s'\n", op);
            break;
        }

        case NodeBlock: {
            LLVMValueRef result = NULL;
            for (int i = 0; i < node->block.count; i++) {
                result = llvm_eval_ast(node->block.statements[i]);
            }
            return result;
        }
        
        case NodePrint: {
            LLVMValueRef val = llvm_eval_ast(node->print.value);
        
            LLVMTypeRef printf_type = NULL;
            printf_func = create_printf_function_type(&printf_type);
        
            LLVMValueRef format_str = NULL;
            LLVMValueRef args[2];
        
            if (strcmp(node->print.type, "int") == 0) {
                format_str = LLVMBuildGlobalStringPtr(Builder, "%ld\n", "fmt");
                args[0] = format_str;
                args[1] = val;
            } else if (strcmp(node->print.type, "float") == 0) {
                format_str = LLVMBuildGlobalStringPtr(Builder, "%lf\n", "fmt");
                args[0] = format_str;
                args[1] = val;
            } else if (strcmp(node->print.type, "char") == 0) {
                format_str = LLVMBuildGlobalStringPtr(Builder, "%c\n", "fmt");
                args[0] = format_str;
                args[1] = val;
            } else if (strcmp(node->print.type, "string") == 0) {
                format_str = LLVMBuildGlobalStringPtr(Builder, "%s\n", "fmt");
                args[0] = format_str;
                args[1] = val;
            } else {
                fprintf(stderr, "LLVM error: unsupported print type '%s'\n", node->print.type);
                break;
            }
        
            LLVMBuildCall2(Builder, printf_type, printf_func, args, 2, "calltmp");
            return NULL;
        }
        

        default:
            fprintf(stderr, "Unsupported AST node type %d\n", node->type);
            break;
    }

    return NULL;
}

LLVMValueRef create_main_function(void) {
    LLVMTypeRef mainFunctionType = LLVMFunctionType(LLVMInt64TypeInContext(TheContext), NULL, 0, 0);
    LLVMValueRef mainFunction = LLVMAddFunction(TheModule, "main", mainFunctionType);
    
    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(TheContext, mainFunction, "entry");
    LLVMPositionBuilderAtEnd(Builder, entry);

    LLVMValueRef result = llvm_eval_ast(root);

    if (result != NULL) {
        LLVMBuildRet(Builder, result);
    } else {
        LLVMValueRef zero = LLVMConstInt(LLVMInt64TypeInContext(TheContext), 0, false);
        LLVMBuildRet(Builder, zero);
    }

    return mainFunction;
}

void write_llvm_ir_to_file(const char *filename) {
    if (LLVMPrintModuleToFile(TheModule, filename, NULL) != 0) {
        fprintf(stderr, "Error writing IR to file %s\n", filename);
    }
}

void print_llvm_ir() {
    char *ir = LLVMPrintModuleToString(TheModule);
    printf("%s\n", ir);
    LLVMDisposeMessage(ir);
}
