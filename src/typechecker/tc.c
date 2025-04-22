#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "memory.h"
#include "tc.h"

extern Arena *global_arena;

static void type_error(const char *msg) {
    fprintf(stderr, "Type error: %s\n", msg);
    exit(1);
}

static TypeMapping primitive_types[] = {
    {"int", TypeInt},
    {"float", TypeFloat},
    {"bool", TypeBool},
    {"char", TypeChar},
    {"string", TypeString}
};

#define NUM_PRIMITIVE_TYPES (sizeof(primitive_types) / sizeof(primitive_types[0]))

TypeTC *make_type(TypeKind kind) {
    TypeTC *t = arena_alloc(global_arena, sizeof(TypeTC));
    t->kind = kind;
    t->element_type = NULL;
    return t;
}

TypeTC *make_list_type(TypeTC *elem_type) {
    TypeTC *t = make_type(TypeList);
    t->element_type = elem_type;
    return t;
}

const char *type_to_string(TypeKind kind) {
    switch (kind) {
        case TypeInt: return "int";
        case TypeFloat: return "float";
        case TypeBool: return "bool";
        case TypeChar: return "char";
        case TypeString: return "string";
        case TypeList: return "list";
        case TypeError: return "<error>";
        default: return "<invalid>";
    }
}

TypeTC *lookup_type_from_string(const char *type_str) {
    for (size_t i = 0; i < (size_t)NUM_PRIMITIVE_TYPES; i++) {
        if (strcmp(primitive_types[i].name, type_str) == 0) {
            return make_type(primitive_types[i].kind);
        }
    }
    return NULL;
}

TypeTC *parse_type_annotation(const char *type_str) {
    TypeTC *base_type = lookup_type_from_string(type_str);
    if (base_type) return base_type;

    if (strncmp(type_str, "list<", 5) == 0 || type_str[0] == '<') {
        char inner[16];
        if (type_str[0] == '<') sscanf(type_str, "<%15[^>]>", inner);
        else sscanf(type_str, "list<%15[^>]>", inner);

        TypeTC *inner_type = lookup_type_from_string(inner);
        if (inner_type) {
            return make_list_type(inner_type);
        }
        type_error("Unknown inner list type");
    }

    fprintf(stderr, "Unknown type annotation: %s\n", type_str);
    exit(1);
}

TypeTC *typecheck_binary(const char *op, TypeTC *left, TypeTC *right) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
        if (left->kind == TypeInt && right->kind == TypeInt)
            return make_type(TypeInt);
        type_error("Operands to '+' must both be int");
    } else if (strcmp(op, "+.") == 0 || strcmp(op, "-.") == 0 ||
               strcmp(op, "*.") == 0 || strcmp(op, "/.") == 0) {
        if (left->kind == TypeFloat && right->kind == TypeFloat)
            return make_type(TypeFloat);
        type_error("Operands to '+.' must both be float");
    } else if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
               strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 ||
               strcmp(op, ">") == 0 || strcmp(op, ">=") == 0) {
        if ((left->kind == TypeInt && right->kind == TypeInt) ||
            (left->kind == TypeFloat && right->kind == TypeFloat)) {
            return make_type(TypeBool);
        }
        type_error("Comparison operators require int or float operands");
    } else if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        if (left->kind == TypeBool && right->kind == TypeBool)
            return make_type(TypeBool);
        type_error("Logical operators require bool operands");
    }

    type_error("Unsupported binary operator");
    return make_type(TypeError);
}

TypeEnv *add_binding(TypeEnv *env, const char *name, TypeTC *type) {
    TypeEnv *new_env = arena_alloc(global_arena, sizeof(TypeEnv));
    new_env->name = name;
    new_env->type = type;
    new_env->next = env;
    return new_env;
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

TypeTC *make_function_type(TypeTC *return_type, TypeTC **param_types, int param_count) {
    TypeTC *t = make_type(TypeFunction);
    t->return_type = return_type;
    t->param_types = param_types;
    t->param_count = param_count;
    return t;
}

TypeTC *typecheck_expr_with_env(ASTNode *node, TypeEnv *env) {
    switch (node->type) {
        case NodeIntLit: return make_type(TypeInt);
        case NodeFloatLit: return make_type(TypeFloat);
        case NodeBoolLit: return make_type(TypeBool);
        case NodeCharLit: return make_type(TypeChar);
        case NodeStringLit: return make_type(TypeString);

        case NodeIdentifier: {
            TypeTC *t = lookup_type(env, node->strval);
            if (!t) {
                fprintf(stderr, "Undefined identifier: %s\n", node->strval);
                exit(1);
            }
            return t;
        }

        case NodeBinaryExpr: {
            TypeTC *left = typecheck_expr_with_env(node->binary_expr.left, env);
            TypeTC *right = typecheck_expr_with_env(node->binary_expr.right, env);
            return typecheck_binary(node->binary_expr.op, left, right);
        }

        case NodeVarDecl: {
            TypeTC *value_type = typecheck_expr_with_env(node->var_decl.expr, env);

            TypeTC *annot_type = NULL;
            if (node->var_decl.type) {
                annot_type = parse_type_annotation(node->var_decl.type);

                if (annot_type->kind != value_type->kind) {
                    type_error("Type mismatch in val binding");
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
                TypeTC *stmt_type = typecheck_expr_with_env(stmt, block_env);
            
                if (stmt->type == NodeVarDecl) {
                    TypeTC *binding_type = stmt_type;
            
                    if (stmt->var_decl.type) {
                        binding_type = parse_type_annotation(stmt->var_decl.type);
                    }
            
                    block_env = add_binding(block_env, stmt->var_decl.value, binding_type);
                }
            
                last_type = stmt_type;
            }                      

            return last_type;
        }

        case NodeList: {
            if (node->list.count == 0) {
                type_error("Cannot infer type of empty list");
            }

            TypeTC *first_elem_type = typecheck_expr_with_env(node->list.elements[0], env);
            for (int i = 1; i < node->list.count; i++) {
                TypeTC *elem_type = typecheck_expr_with_env(node->list.elements[i], env);
                if (elem_type->kind != first_elem_type->kind) {
                    type_error("All list elements must have the same type");
                }
            }

            return make_list_type(first_elem_type);
        }

        case NodePrint: {
            TypeTC *annot_type = parse_type_annotation(node->print.type);
            TypeTC *value = typecheck_expr_with_env(node->print.value, env);

            if (annot_type->kind != value->kind) {
                fprintf(stderr, "Type error: print expected type <%s> but got <%s>\n", type_to_string(annot_type->kind), type_to_string(value->kind));
                exit(1);
            }
            return value;
        }

        case NodeFunction: {
            TypeTC *return_type = parse_type_annotation(node->function.return_type);

            TypeTC **param_types = arena_alloc(global_arena, sizeof(TypeTC*) * (size_t)node->function.param_count);
            for (int i = 0; i < node->function.param_count; i++) {
                param_types[i] = parse_type_annotation(node->function.param_types[i]);
            }

            TypeTC *function_type = make_function_type(return_type, param_types, node->function.param_count);
            env = add_binding(env, node->function.name, function_type);

            TypeEnv *function_env = env;
            for (int i = 0; i < node->function.param_count; i++) {
                function_env = add_binding(function_env, node->function.param_names[i], param_types[i]);
            }
            TypeTC *body_type = typecheck_expr_with_env(node->function.expr, function_env);

            if (return_type->kind != body_type->kind) {
                fprintf(stderr, "Function '%s' returns type <%s> but body evaluates to <%s>\n",
                        node->function.name, type_to_string(return_type->kind), type_to_string(body_type->kind));
                exit(1);
            }

            return return_type;
        }

        case NodeCall: {
            TypeTC *callee_type = typecheck_expr_with_env(node->call.callee, env);
            if (callee_type->kind != TypeFunction) {
                type_error("Callee must be a function");
            }

            TypeTC **param_types = callee_type->param_types;
            int param_count = callee_type->param_count;

            if (node->call.arg_count != param_count) {
                type_error("Argument count mismatch in function call");
            }

            for (int i = 0; i < node->call.arg_count; i++) {
                TypeTC *arg_type = typecheck_expr_with_env(node->call.args[i], env);
                if (arg_type->kind != param_types[i]->kind) {
                    fprintf(stderr, "Type mismatch in argument %d: expected <%s> but got <%s>\n",
                            i + 1, type_to_string(param_types[i]->kind), type_to_string(arg_type->kind));
                    exit(1);
                }
            }

            return callee_type->return_type;
        }
        
        default:
            type_error("Unsupported expression type");
    }

    return make_type(TypeError);
}

TypeTC *typecheck(ASTNode *node) {
    if (node->type != NodeBlock) {
        return typecheck_expr_with_env(node, NULL);
    }

    TypeEnv *env = NULL;

    for (int i = 0; i < node->block.count; i++) {
        ASTNode *stmt = node->block.statements[i];

        if (stmt->type == NodeFunction) {
            TypeTC *return_type = parse_type_annotation(stmt->function.return_type);
            TypeTC **param_types = arena_alloc(global_arena, sizeof(TypeTC*) * (size_t)stmt->function.param_count);
            for (int j = 0; j < stmt->function.param_count; j++) {
                param_types[j] = parse_type_annotation(stmt->function.param_types[j]);
            }
            TypeTC *func_type = make_function_type(return_type, param_types, stmt->function.param_count);
            env = add_binding(env, stmt->function.name, func_type);
        }

        if (stmt->type == NodeVarDecl) {
            TypeTC *value_type = NULL;
            if (stmt->var_decl.type) {
                value_type = parse_type_annotation(stmt->var_decl.type);
            } else {
                value_type = typecheck_expr_with_env(stmt->var_decl.expr, env);
            }
            env = add_binding(env, stmt->var_decl.value, value_type);
        }
    }

    TypeTC *last_type = make_type(TypeError);
    for (int i = 0; i < node->block.count; i++) {
        last_type = typecheck_expr_with_env(node->block.statements[i], env);
    }

    return last_type;
}
