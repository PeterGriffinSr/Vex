#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    NodeIntLit,
    NodeFloatLit,
    NodeStringLit,
    NodeCharLit,
    NodeIdentifier,
    NodeVarDecl,
    NodeBinaryExpr
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;

    union {
        int intval;
        double floatval;
        char charval;
        const char *strval;

        struct {
            const char *op;
            ASTNode *left, *right;
        } binary_expr;

        struct {
            const char *value, *type;
            ASTNode *expr;
        } var_decl;
    };
};

ASTNode *alloc_node(NodeType type);
ASTNode *create_int_node(int value);
ASTNode *create_char_node(char value);
ASTNode *create_float_node(double value);
ASTNode *create_string_node(const char *value);
ASTNode *create_identifier_node(const char *value);
ASTNode *create_binary_node(const char *op, ASTNode *left, ASTNode *right);
ASTNode *create_var_decl_node(const char* value, const char *type, ASTNode *expr);

void printAST(ASTNode *node, int indent);

#endif // AST_H
