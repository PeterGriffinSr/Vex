#ifndef AST_H
#define AST_H

#include <stddef.h>
typedef enum {
    NodeIntLit,
    NodeFloatLit,
    NodeStringLit,
    NodeCharLit,
    NodeIdentifier,
    NodeBinaryExpr
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;

    union {
        int intval;
        double floatval;
        char charval;
        char *strval;
        struct {
            const char *op;
            ASTNode *left, *right;
        } binary_expr;
    };
};

ASTNode *create_int_node(int value);
ASTNode *create_float_node(double value);
ASTNode *create_char_node(char value);
ASTNode *create_string_node(char *value);
ASTNode *create_identifier_node(char *value);
ASTNode *create_binary_node(const char *op, ASTNode *left, ASTNode *right);
void printAST(ASTNode *node, int indent);
void freeAST(ASTNode *node);

#endif // AST_H
