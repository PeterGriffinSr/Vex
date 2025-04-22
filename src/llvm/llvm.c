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
static VarBinding *variables = NULL;

void insert_variable(const char *name, LLVMValueRef value) {
    VarBinding *entry = malloc(sizeof(VarBinding));
    entry->name = name;
    entry->value = value;
    HASH_ADD_KEYPTR(hh, variables, entry->name, strlen(entry->name), entry);
}

LLVMValueRef get_variable(const char *name) {
    VarBinding *entry;
    HASH_FIND_STR(variables, name, entry);
    return entry ? entry->value : NULL;
}

void free_variables(void) {
    VarBinding *current, *tmp;
    HASH_ITER(hh, variables, current, tmp) {
        HASH_DEL(variables, current);
        free(current);
    }
}

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

        case NodeBoolLit: {
            return LLVMConstInt(LLVMInt1TypeInContext(TheContext), node->boolval ? 1 : 0, false);
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
            } else if (strcmp(node->print.type, "bool") == 0) {
                format_str = LLVMBuildGlobalStringPtr(Builder, "%d\n", "fmt");
                args[0] = format_str;
                args[1] = LLVMBuildZExt(Builder, val, LLVMInt32TypeInContext(TheContext), "bool2i32");
            } else {
                fprintf(stderr, "LLVM error: unsupported print type '%s'\n", node->print.type);
                break;
            }
        
            LLVMBuildCall2(Builder, printf_type, printf_func, args, 2, "calltmp");
            return LLVMConstInt(LLVMInt64TypeInContext(TheContext), 0, false);
        }
        
        case NodeVarDecl : {
            LLVMValueRef init = llvm_eval_ast(node->var_decl.expr);
            LLVMTypeRef type;

            if (strcmp(node->var_decl.type, "int") == 0) {
                type = LLVMInt64TypeInContext(TheContext);
            } else if (strcmp(node->var_decl.type, "float") == 0) {
                type = LLVMDoubleTypeInContext(TheContext);
            } else if (strcmp(node->var_decl.type, "char") == 0) {
                type = LLVMInt8TypeInContext(TheContext);
            } else if (strcmp(node->var_decl.type, "string") == 0) {
                type = LLVMPointerType(LLVMInt8TypeInContext(TheContext), 0);
            } else if (strcmp(node->var_decl.type, "bool") == 0) {
                type = LLVMInt1TypeInContext(TheContext);
            } else {
                fprintf(stderr, "LLVM error: unknown variable type '%s'\n", node->var_decl.type);
                break;
            }

            LLVMValueRef alloc = LLVMBuildAlloca(Builder, type, node->var_decl.value);
            LLVMBuildStore(Builder, init, alloc);
            insert_variable(node->var_decl.value, alloc);
            return alloc;
        }

        case NodeIdentifier: {
            LLVMValueRef alloc = get_variable(node->strval);
            if (!alloc) {
                fprintf(stderr, "LLVM error: unknown identifier '%s'\n", node->strval);
                break;
            }
            LLVMTypeRef elem_type = LLVMGetAllocatedType(alloc);
            return LLVMBuildLoad2(Builder, elem_type, alloc, "loadtmp");
        }

        case NodeFunction: {
            LLVMTypeRef *param_types = malloc(sizeof(LLVMTypeRef) * (size_t)node->function.param_count);
            for (int i = 0; i < node->function.param_count; i++) {
                const char *type_str = node->function.param_types[i];
                if (strcmp(type_str, "int") == 0) {
                    param_types[i] = LLVMInt64TypeInContext(TheContext);
                } else if (strcmp(type_str, "float") == 0) {
                    param_types[i] = LLVMDoubleTypeInContext(TheContext);
                } else if (strcmp(type_str, "char") == 0) {
                    param_types[i] = LLVMInt8TypeInContext(TheContext);
                } else if (strcmp(type_str, "string") == 0) {
                    param_types[i] = LLVMPointerType(LLVMInt8TypeInContext(TheContext), 0);
                } else if (strcmp(type_str, "bool") == 0) {
                    param_types[i] = LLVMInt1TypeInContext(TheContext);
                } else {
                    fprintf(stderr, "LLVM error: unsupported parameter type '%s'\n", type_str);
                    return NULL;
                }
            }

            LLVMTypeRef ret_type;
            if (strcmp(node->function.return_type, "int") == 0) {
                ret_type = LLVMInt64TypeInContext(TheContext);
            } else if (strcmp(node->function.return_type, "float") == 0) {
                ret_type = LLVMDoubleTypeInContext(TheContext);
            } else if (strcmp(node->function.return_type, "char") == 0) {
                ret_type = LLVMInt8TypeInContext(TheContext);
            } else if (strcmp(node->function.return_type, "string") == 0) {
                ret_type = LLVMPointerType(LLVMInt8TypeInContext(TheContext), 0);
            } else if (strcmp(node->function.return_type, "bool") == 0) {
                ret_type = LLVMInt1TypeInContext(TheContext);
            } else {
                fprintf(stderr, "LLVM error: unsupported return type '%s'\n", node->function.return_type);
                return NULL;
            }

            LLVMTypeRef func_type = LLVMFunctionType(ret_type, param_types, (unsigned int)node->function.param_count, 0);
            LLVMValueRef function = LLVMAddFunction(TheModule, node->function.name, func_type);

            LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(TheContext, function, "entry");
            LLVMPositionBuilderAtEnd(Builder, entry);

            for (int i = 0; i < node->function.param_count; i++) {
                LLVMValueRef param = LLVMGetParam(function, (unsigned int)i);
                LLVMValueRef alloca = LLVMBuildAlloca(Builder, param_types[i], node->function.param_names[i]);
                LLVMBuildStore(Builder, param, alloca);
                insert_variable(node->function.param_names[i], alloca);
            }

            LLVMValueRef body = llvm_eval_ast(node->function.expr);
            LLVMBuildRet(Builder, body);
            free_variables();

            return function;
        }

        case NodeCall: {
            LLVMValueRef callee = llvm_eval_ast(node->call.callee);
            if (!callee) {
                fprintf(stderr, "LLVM error: failed to evaluate function callee\n");
                return NULL;
            }

            LLVMTypeRef func_type = LLVMGetElementType(LLVMTypeOf(callee));
            unsigned param_count = (unsigned int)node->call.arg_count;
            LLVMValueRef *args = malloc(sizeof(LLVMValueRef) * param_count);
            for (unsigned int i = 0; i < param_count; i++) {
                args[i] = llvm_eval_ast(node->call.args[i]);
                if (!args[i]) {
                    fprintf(stderr, "LLVM error: failed to evaluate argument %d\n", i);
                    free(args);
                    return NULL;
                }
            }

            return LLVMBuildCall2(Builder, func_type, callee, args, param_count, "calltmp");
        }

        default:
            fprintf(stderr, "Unsupported AST node type %d\n", node->type);
            break;
    }

    return NULL;
}

void compile_root(void) {
    if (!root) return;
    llvm_eval_ast(root);
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
