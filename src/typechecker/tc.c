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
                last_type = typecheck_expr_with_env(stmt, block_env);

                if (stmt->type == NodeVarDecl) {
                    TypeTC *binding_type = last_type;

                    if (stmt->var_decl.type) {
                        binding_type = parse_type_annotation(stmt->var_decl.type);
                    }

                    block_env = add_binding(block_env, stmt->var_decl.value, binding_type);
                }
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

        default:
            type_error("Unsupported expression type");
    }

    return make_type(TypeError);
}

TypeTC *typecheck(ASTNode *node) {
    return typecheck_expr_with_env(node, NULL);
}
