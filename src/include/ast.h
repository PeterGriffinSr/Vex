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
    NodeList,
    NodeUnaryExpr,
    NodeBlock,
    NodeIf,
    NodeFunction,
    NodeCall,
    NodeBinaryExpr
} NodeType;

typedef struct ASTNode ASTNode;

struct Param {
    const char *name;
    const char *type;
};

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
            ASTNode **elements;
            int count;
        } list;

        struct {
            ASTNode **statements;
            int count;
        } block;

        struct {
            ASTNode *condition, *then_branch, *else_branch;
        } if_stmt;

        struct {
            const char *name;
            const char **param_names;
            const char **param_types;
            int param_count;
            int is_recursive;
            ASTNode *expr;
        } function;

        struct {
            ASTNode *callee;
            ASTNode **args;
            int arg_count;
        } call;
    };
};

ASTNode *alloc_node(NodeType type);
ASTNode *create_int_node(int value);
ASTNode *create_bool_node(int value);
ASTNode *create_char_node(char value);
ASTNode *create_float_node(double value);
ASTNode *create_string_node(const char *value);
ASTNode *create_identifier_node(const char *value);
ASTNode *create_block_node(ASTNode **stmts, int count);
ASTNode *create_list_node(ASTNode **elements, int count);
ASTNode *create_unary_node(const char *op, ASTNode *operand);
ASTNode *create_call_node(ASTNode *callee, ASTNode **args, int arg_count);
ASTNode *create_binary_node(const char *op, ASTNode *left, ASTNode *right);
ASTNode *create_var_decl_node(const char* value, const char *type, ASTNode *expr);
ASTNode *create_if_stmt_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode *create_function_node(const char *name, struct Param *params, int param_count, int is_recursive, ASTNode *expr);

void printAST(ASTNode *node, int indent);
ASTNode *build_list(ASTNode **items, int count);
void indent_print(int indent, const char *fmt, ...);

#endif // AST_H
