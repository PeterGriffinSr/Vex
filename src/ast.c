#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/ast.h"
#include "include/memory.h"

extern Arena *global_arena;

ASTNode *alloc_node(NodeType type) {
    ASTNode *node = arena_alloc(global_arena, sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode *create_int_node(int value) {
    ASTNode *node = alloc_node(NodeIntLit);
    node->intval = value;
    return node;   
}

ASTNode *create_float_node(double value) {
    ASTNode *node = alloc_node(NodeFloatLit);
    node->floatval = value;
    return node;
}

ASTNode *create_char_node(char value) {
    ASTNode *node = alloc_node(NodeCharLit);
    node->charval = value;
    return node;
}

ASTNode *create_string_node(const char *value) {
    ASTNode *node = alloc_node(NodeStringLit);
    size_t len = strlen(value) + 1;
    char *copy = arena_alloc(global_arena, len);
    memcpy(copy, value, len);
    node->strval = copy;
    return node;
}

ASTNode *create_identifier_node(const char *value) {
    ASTNode *node = alloc_node(NodeIdentifier);
    size_t len = strlen(value) + 1;
    char *copy = arena_alloc(global_arena, len);
    memcpy(copy, value, len);
    node->strval = copy;
    return node;
}

ASTNode *create_var_decl_node(const char *value, const char *type, ASTNode *expr) {
    ASTNode *node = alloc_node(NodeVarDecl);
    node->var_decl.value = value;
    node->var_decl.type = type;
    node->var_decl.expr = expr;
    return node;
}

ASTNode *create_binary_node(const char *op, ASTNode *left, ASTNode *right) {
    ASTNode *node = alloc_node(NodeBinaryExpr);
    node->binary_expr.op = op;
    node->binary_expr.left = left;
    node->binary_expr.right = right;
    return node;
}

void printAST(ASTNode *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    switch (node->type) {
        case NodeIntLit:
            printf("IntLiteral: %d\n", node->intval);
            break;
        case NodeFloatLit:
            printf("FloatLiteral: %lf\n", node->floatval);
            break;
        case NodeCharLit:
            printf("CharLiteral: '%c'\n", node->charval);
            break;
        case NodeStringLit:
            printf("StringLiteral: %s\n", node->strval);
            break;
        case NodeIdentifier:
            printf("Identifier: %s\n", node->strval);
            break;
        case NodeBinaryExpr:
            printf("BinaryOp: '%s'\n", node->binary_expr.op);
            printAST(node->binary_expr.left, indent + 1);
            printAST(node->binary_expr.right, indent + 1);
            break;
        case NodeVarDecl:
            printf("VarDecl: ");
            printf("Type: %s, ", node->var_decl.type ? node->var_decl.type : "<inferred>");
            printf("Identifier: %s", node->var_decl.value);
            if (node->var_decl.expr) {
                printf(" =\n");
                printAST(node->var_decl.expr, indent + 1);
            }
            break;
        default:
            return;
    }
}
