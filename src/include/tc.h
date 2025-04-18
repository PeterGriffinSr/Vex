#ifndef TC_H
#define TC_H

#include "ast.h"

typedef enum {
    TypeInt,
    TypeFloat,
    TypeBool,
    TypeChar,
    TypeString,
    TypeError
} TypeKind;

typedef struct {
    TypeKind kind;
} TypeTC;

typedef struct TypeEnv {
    const char *name;
    TypeTC *type;
    struct TypeEnv *next;
} TypeEnv;

TypeTC *lookup_type(TypeEnv *env, const char *name);
TypeEnv *add_binding(TypeEnv *env, const char *name, TypeTC *type);
TypeTC *typecheck(ASTNode *node);
TypeTC *typecheck_expr_with_env(ASTNode *node, TypeEnv *env) ;
const char *type_to_string(TypeKind kind);
TypeTC *make_type(TypeKind kind)  ;
TypeTC *typecheck_binary(const char *op, TypeTC *left, TypeTC *right);
TypeTC *typecheck_function(ASTNode *node, TypeEnv *parent_env);
TypeTC *typecheck_expr(ASTNode *node);
void update_binding(TypeEnv *env, ASTNode *ident_node, TypeTC *new_type);
TypeTC *infer_from_binary_op(const char *op, TypeTC *other);

#endif // TC_H
