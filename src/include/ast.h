#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    NodeIntLit,
    NodeFloatLit,
    NodeStringLit,
    NodeCharLit,
    NodeBoolLit,
    NodeIdentifier,
    NodeVarDecl,
    NodeUnaryExpr,
    NodeBlock,
    NodePrint,
    NodeList,
    NodeBinaryExpr
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;

    union {
        int intval;
        int boolval;
        double floatval;
        char charval;
        const char *strval;

        struct {
            const char *op;
            ASTNode *left, *right;
        } binary_expr;

        struct {
            const char *op;
            ASTNode *operand;
        } unary_expr;

        struct {
            const char *value, *type;
            ASTNode *expr;
        } var_decl;

        struct {
            ASTNode **statements;
            int count;
        } block;

        struct {
            ASTNode *value;
            const char *type;
        } print;

        struct {
            ASTNode **elements;
            int count;
        } list;
    };
};

ASTNode *alloc_node(NodeType type);
ASTNode *create_int_node(int value);
ASTNode *create_bool_node(int value);
ASTNode *create_char_node(char value);
ASTNode *create_float_node(double value);
ASTNode *create_string_node(const char *value);
ASTNode *build_list(ASTNode **items, int count);
ASTNode *create_identifier_node(const char *value);
ASTNode *create_block_node(ASTNode **stmts, int count);
ASTNode *create_list_node(ASTNode **elements, int count);
ASTNode *create_print_node(ASTNode *value, const char *type);
ASTNode *create_unary_node(const char *op, ASTNode *operand);
ASTNode *create_binary_node(const char *op, ASTNode *left, ASTNode *right);
ASTNode *create_var_decl_node(const char* value, const char *type, ASTNode *expr);

void printAST(ASTNode *node, int indent);
void indent_print(int indent, const char *fmt, ...);

#endif // AST_H
