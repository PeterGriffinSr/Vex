#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tc.h"
#include "memory.h"

extern Arena *global_arena;

static void type_error(const char *msg) {
    fprintf(stderr, "Type error: %s\n", msg);
    exit(1);
}

TypeTC *make_type(TypeKind kind) {
    TypeTC *t = arena_alloc(global_arena, sizeof(TypeTC));
    t->kind = kind;
    return t;
}

const char *type_to_string(TypeKind kind) {
    switch (kind) {
        case TypeInt: return "int";
        case TypeFloat: return "float";
        case TypeBool: return "bool";
        case TypeChar: return "char";
        case TypeString: return "string";
        case TypeError: return "<error>";
        default: return "<invalid>";
    }
}

TypeTC *typecheck_binary(const char *op, TypeTC *left, TypeTC *right) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
        if (left->kind == TypeInt && right->kind == TypeInt)
            return make_type(TypeInt);
        type_error("Operands to '+' (int ops) must both be int");
    } else if (strcmp(op, "+.") == 0 || strcmp(op, "-.") == 0 ||
               strcmp(op, "*.") == 0 || strcmp(op, "/.") == 0) {
        if (left->kind == TypeFloat && right->kind == TypeFloat)
            return make_type(TypeFloat);
        type_error("Operands to '+.' (float ops) must both be float");
    } else if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
               strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 ||
               strcmp(op, ">") == 0 || strcmp(op, ">=") == 0) {
        if ((left->kind == TypeInt && right->kind == TypeInt) ||
            (left->kind == TypeFloat && right->kind == TypeFloat)) {
            return make_type(TypeBool);
        }
        type_error("Comparison operators require both operands to be int or float");
    } else if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        if (left->kind == TypeBool && right->kind == TypeBool)
            return make_type(TypeBool);
        type_error("Logical operators require both operands to be bool");
    }

    type_error("Unsupported binary operator");
    return make_type(TypeError);
}

TypeTC *typecheck_function(ASTNode *node, TypeEnv *parent_env) {
    TypeEnv *env = parent_env;

    for (int i = 0; i < node->function.param_count; i++) {
        const char *annot = node->function.param_types[i];
        TypeTC *param_type = NULL;

        if (annot) {
            if (strcmp(annot, "int") == 0) {
                param_type = make_type(TypeInt);
            } else if (strcmp(annot, "float") == 0) {
                param_type = make_type(TypeFloat);
            } else if (strcmp(annot, "bool") == 0) {
                param_type = make_type(TypeBool);
            } else if (strcmp(annot, "char") == 0) {
                param_type = make_type(TypeChar);
            } else if (strcmp(annot, "string") == 0) {
                param_type = make_type(TypeString);
            } else {
                type_error("Unknown parameter type annotation");
            }
        } else {
            param_type = make_type(TypeError);
        }

        env = add_binding(env, node->function.param_names[i], param_type);
    }

    TypeTC *ret_type = typecheck_expr_with_env(node->function.expr, env);

    for (int i = 0; i < node->function.param_count; i++) {
        const char *param_name = node->function.param_names[i];
        TypeTC *param_type = lookup_type(env, param_name);

        if (param_type->kind == TypeError) {
            ASTNode *expr = node->function.expr;

            if (expr->type == NodeBinaryExpr) {
                TypeTC *left_type = typecheck_expr_with_env(expr->binary_expr.left, env);
                TypeTC *right_type = typecheck_expr_with_env(expr->binary_expr.right, env);

                if (expr->binary_expr.left->type == NodeIdentifier && 
                    strcmp(expr->binary_expr.left->strval, param_name) == 0) {
                    param_type = left_type;
                } else if (expr->binary_expr.right->type == NodeIdentifier && 
                           strcmp(expr->binary_expr.right->strval, param_name) == 0) {
                    param_type = right_type;
                }
            }

            if (param_type->kind == TypeError) {
                type_error("Unable to infer parameter type from function body");
            }

            env = add_binding(env, param_name, param_type);
        }
    }

    return ret_type;
}

TypeTC *infer_from_binary_op(const char *op, TypeTC *other) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0)
        return other->kind == TypeInt ? make_type(TypeInt) : make_type(TypeError);

    if (strcmp(op, "+.") == 0 || strcmp(op, "-.") == 0 ||
        strcmp(op, "*.") == 0 || strcmp(op, "/.") == 0)
        return other->kind == TypeFloat ? make_type(TypeFloat) : make_type(TypeError);

    return make_type(TypeError);
}

void update_binding(TypeEnv *env, ASTNode *ident_node, TypeTC *new_type) {
    if (ident_node->type != NodeIdentifier) return;

    while (env) {
        if (strcmp(env->name, ident_node->strval) == 0 && env->type->kind == TypeError) {
            env->type = new_type;
            return;
        }
        env = env->next;
    }
}

TypeTC *typecheck_expr_with_env(ASTNode *node, TypeEnv *env) {
    switch (node->type) {
        case NodeIntLit: return make_type(TypeInt);
        case NodeFloatLit: return make_type(TypeFloat);
        case NodeBoolLit: return make_type(TypeBool);
        case NodeCharLit: return make_type(TypeChar);
        case NodeStringLit: return make_type(TypeString);

        case NodeBinaryExpr: {
            TypeTC *left = typecheck_expr_with_env(node->binary_expr.left, env);
            TypeTC *right = typecheck_expr_with_env(node->binary_expr.right, env);

            if (left->kind == TypeError && right->kind != TypeError) {
                TypeTC *inferred = infer_from_binary_op(node->binary_expr.op, right);
                update_binding(env, node->binary_expr.left, inferred);
                left = inferred;
            }
        
            if (right->kind == TypeError && left->kind != TypeError) {
                TypeTC *inferred = infer_from_binary_op(node->binary_expr.op, left);
                update_binding(env, node->binary_expr.right, inferred);
                right = inferred;
            }

            return typecheck_binary(node->binary_expr.op, left, right);
        }
        case NodeFunction:
            return typecheck_function(node, env);
        case NodeIdentifier: {
            TypeTC *t = lookup_type(env, node->strval);
            if (!t) {
                fprintf(stderr, "Undefined identifier: %s\n", node->strval);
                exit(1);
            }
            return t;
        } 
        case NodeVarDecl: {
            TypeTC *value_type = typecheck_expr_with_env(node->var_decl.expr, env);
        
            if (node->var_decl.type) {
                TypeTC *annot_type = NULL;
                if (strcmp(node->var_decl.type, "int") == 0) {
                    annot_type = make_type(TypeInt);
                } else if (strcmp(node->var_decl.type, "float") == 0) {
                    annot_type = make_type(TypeFloat);
                } else if (strcmp(node->var_decl.type, "bool") == 0) {
                    annot_type = make_type(TypeBool);
                } else if (strcmp(node->var_decl.type, "char") == 0) {
                    annot_type = make_type(TypeChar);
                } else if (strcmp(node->var_decl.type, "string") == 0) {
                    annot_type = make_type(TypeString);
                } else {
                    type_error("Unknown type annotation in let binding");
                }
        
                if (annot_type->kind != value_type->kind) {
                    type_error("Type mismatch in let binding");
                }
        
                env = add_binding(env, node->var_decl.value, annot_type);
                return annot_type;
            } else {
                env = add_binding(env, node->var_decl.value, value_type);
                return value_type;
            }
        } 
        case NodeBlock: {
            TypeEnv *block_env = env;
            TypeTC *last_type = make_type(TypeError);
        
            for (int i = 0; i < node->block.count; i++) {
                ASTNode *stmt = node->block.statements[i];
                last_type = typecheck_expr_with_env(stmt, block_env);
        
                if (stmt->type == NodeVarDecl) {
                    TypeTC *binding_type = last_type;
        
                    if (stmt->var_decl.type) {
                        if (strcmp(stmt->var_decl.type, "int") == 0) {
                            binding_type = make_type(TypeInt);
                        } else if (strcmp(stmt->var_decl.type, "float") == 0) {
                            binding_type = make_type(TypeFloat);
                        } else if (strcmp(stmt->var_decl.type, "bool") == 0) {
                            binding_type = make_type(TypeBool);
                        } else if (strcmp(stmt->var_decl.type, "char") == 0) {
                            binding_type = make_type(TypeChar);
                        } else if (strcmp(stmt->var_decl.type, "string") == 0) {
                            binding_type = make_type(TypeString);
                        } else {
                            type_error("Unknown type annotation in block let binding");
                        }
                    }
        
                    block_env = add_binding(block_env, stmt->var_decl.value, binding_type);
                }
            }
        
            return last_type;
        }
        
        default:
            type_error("Unsupported expression type");
    }
    return make_type(TypeError);
}

TypeTC *lookup_type(TypeEnv *env, const char *name) {
    while (env) {
        if (strcmp(env->name, name) == 0) {
            return env->type;
        }
        env = env->next;
    }
    return NULL;
}

TypeEnv *add_binding(TypeEnv *env, const char *name, TypeTC *type) {
    TypeEnv *new_env = arena_alloc(global_arena, sizeof(TypeEnv));
    new_env->name = name;
    new_env->type = type;
    new_env->next = env;
    return new_env;
}

TypeTC *typecheck(ASTNode *node) {
    return typecheck_expr_with_env(node, NULL);
}
