#ifndef TC_H
#define TC_H

#include "ast.h"

typedef enum {
    TypeInt,
    TypeFloat,
    TypeBool,
    TypeChar,
    TypeString,
    TypeList,
    TypeError
} TypeKind;

typedef struct TypeTC TypeTC;

struct TypeTC {
    TypeKind kind;
    TypeTC *element_type;
};

typedef struct TypeEnv {
    const char *name;
    TypeTC *type;
    struct TypeEnv *next;
} TypeEnv;

typedef struct TypeMapping {
    const char *name;
    TypeKind kind;
} TypeMapping;

TypeTC *typecheck(ASTNode *node);
TypeTC *make_type(TypeKind kind);
TypeTC *typecheck_expr(ASTNode *node);
const char *type_to_string(TypeKind kind);
TypeTC *make_list_type(TypeTC *elem_type);
TypeTC *parse_type_annotation(const char *type_str);
TypeTC *lookup_type(TypeEnv *env, const char *name);
TypeTC *lookup_type_from_string(const char *type_str);
TypeTC *infer_from_binary_op(const char *op, TypeTC *other);
TypeTC *typecheck_expr_with_env(ASTNode *node, TypeEnv *env);
TypeTC *typecheck_function(ASTNode *node, TypeEnv *parent_env);
TypeEnv *add_binding(TypeEnv *env, const char *name, TypeTC *type);
TypeTC *typecheck_binary(const char *op, TypeTC *left, TypeTC *right);
void update_binding(TypeEnv *env, ASTNode *ident_node, TypeTC *new_type);

#endif // TC_H
