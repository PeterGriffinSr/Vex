#if defined (__linux__) || defined (__unix__) || defined (__APPLE__)
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/ast.h"

ASTNode *create_int_node(int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NodeIntLit;
    node->intval = value;
    return node;   
}

ASTNode *create_float_node(double value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NodeFloatLit;
    node->floatval = value;
    return node;
}

ASTNode *create_char_node(char value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NodeCharLit;
    node->charval = value;
    return node;
}

ASTNode *create_string_node(char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NodeStringLit;
    node->strval = strdup(value);
    return node;
}

ASTNode *create_identifier_node(char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NodeIdentifier;
    node->strval = strdup(value);
    return node;
}

ASTNode *create_binary_node(const char *op, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NodeBinaryExpr;
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
        default:
            return;
    }
}

void freeAST(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NodeIntLit:
        case NodeFloatLit:
        case NodeCharLit:
            break;
        case NodeStringLit:
            free(node->strval);
            break;
        case NodeIdentifier:
            free(node->strval);
            break;
        case NodeBinaryExpr:
            freeAST(node->binary_expr.left);
            freeAST(node->binary_expr.right);
            break;
        default:
            break;
    }

    free(node);
}
