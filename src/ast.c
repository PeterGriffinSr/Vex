#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"
#include "memory.h"

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

ASTNode *create_bool_node(int value) {
    ASTNode *node = alloc_node(NodeBoolLit);
    node->boolval = value;
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

ASTNode *create_list_node(ASTNode **elements, int count) {
    ASTNode *node = alloc_node(NodeList);
    node->list.elements = elements;
    node->list.count = count;
    return node;
}

ASTNode *build_list(ASTNode **items, int count) {
    return create_list_node(items, count);
}

ASTNode *create_unary_node(const char *op, ASTNode *operand) {
    ASTNode *node = alloc_node(NodeUnaryExpr);
    node->unary_expr.op = op;
    node->unary_expr.operand = operand;
    return node;
}

ASTNode *create_block_node(ASTNode **stmts, int count) {
    ASTNode *node = arena_alloc(global_arena, sizeof(ASTNode));
    node->type = NodeBlock;
    node->block.statements = stmts;
    node->block.count = count;
    return node;
}

ASTNode *create_if_stmt_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch) {
    ASTNode *node = alloc_node(NodeIf);
    node->type = NodeIf;
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    return node;
}

ASTNode *create_function_node(const char *name, struct Param *params, int param_count, int is_recursive, ASTNode *expr) {
    ASTNode *node = alloc_node(NodeFunction);
    node->function.name = name;
    node->function.param_count = param_count;
    node->function.is_recursive = is_recursive;
    node->function.expr = expr;
    if (params) {
        const char **names = arena_alloc(global_arena, sizeof(char *) * (size_t)param_count);
        const char **types = arena_alloc(global_arena, sizeof(char *) * (size_t)param_count);
        for (int i = 0; i < param_count; i++) {
            names[i] = params[i].name;
            types[i] = params[i].type;
        }
        node->function.param_names = names;
        node->function.param_types = types;
    } else {
        node->function.param_names = NULL;
        node->function.param_types = NULL;
    }
    return node;
}

ASTNode *create_call_node(ASTNode *callee, ASTNode **args, int arg_count) {
    ASTNode *node = alloc_node(NodeCall);
    node->type = NodeCall;
    node->call.callee = callee;
    node->call.args = args;
    node->call.arg_count = arg_count;
    return node;
}

void indent_print(int indent, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    for (int i = 0; i < indent; i++) printf("  ");
    vprintf(fmt, args);
    va_end(args);
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
        case NodeBoolLit:
            printf("BoolLiteral: %d\n", node->boolval);
            break;
        case NodeIdentifier:
            printf("Identifier: %s\n", node->strval);
            break;
        case NodeBinaryExpr:
            printf("BinaryOp: '%s'\n", node->binary_expr.op);
            printAST(node->binary_expr.left, indent + 1);
            printAST(node->binary_expr.right, indent + 1);
            break;
        case NodeUnaryExpr:
            printf("UnaryExpr: '%s'\n", node->unary_expr.op);
            printAST(node->unary_expr.operand, indent + 1);
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
        case NodeList:
            printf("List:\n");
            for (int i = 0; i < node->list.count; i++) {
                printAST(node->list.elements[i], indent + 1);
            }
            break;
        case NodeBlock:
            printf("Block:\n");
            for (int i = 0; i < node->block.count; i++) {
                printAST(node->block.statements[i], indent + 1);
            }
            break;
        case NodeIf:
            printf("If:\n");
            indent_print(indent + 1, "Condition:\n");
            printAST(node->if_stmt.condition, indent + 2);
            indent_print(indent, "Then:\n");
            printAST(node->if_stmt.then_branch, indent + 2);
            indent_print(indent, "Else:\n");
            printAST(node->if_stmt.else_branch, indent + 2);
            break;
        case NodeFunction:
            indent_print(indent, "Function: %s%s\n", node->function.name, node->function.is_recursive ? " (rec)" : "");
            if (node->function.param_count > 0) {
                indent_print(indent + 1, "Params:\n");
                for (int i = 0; i < node->function.param_count; i++) {
                    if (node->function.param_types && node->function.param_types[i])
                        indent_print(indent + 2, "%s: %s\n", node->function.param_names[i], node->function.param_types[i]);
                    else
                        indent_print(indent + 2, "%s\n", node->function.param_names[i]);
                }
            }
            indent_print(indent + 1, "Body:\n");
            printAST(node->function.expr, indent + 2);
            break; 
        case NodeCall:
            printf("Call:\n");
            printAST(node->call.callee, indent + 1);
            for (int i = 0; i < node->call.arg_count; i++) {
                indent_print(indent + 1, NULL);
                printf("Arg %d:\n", i);
                printAST(node->call.args[i], indent + 2);
            }
            break;
                   
        default:
            return;
    }
}
