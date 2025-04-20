#include <stdio.h>
#include <string.h>
#include "eval.h"
#include "ast.h"

Value eval_ast(ASTNode *node) {
    Value result = { .kind = VAL_UNIT };

    switch (node->type) {
        case NodeIntLit: {
            result.kind = VAL_INT;
            result.int_val = node->intval;
            break;
        }

        case NodeFloatLit: {
            result.kind = VAL_FLOAT;
            result.float_val = node->floatval;
            break;
        }

        case NodeStringLit: {
            result.kind = VAL_STRING;
            result.string_val = node->strval;
            break;
        }

        case NodeCharLit: {
            result.kind = VAL_CHAR;
            result.char_val = node->charval;
            break;
        }

        case NodeBinaryExpr: {
            Value left = eval_ast(node->binary_expr.left);
            Value right = eval_ast(node->binary_expr.right);
            const char *op = node->binary_expr.op;

            if (left.kind == VAL_INT && right.kind == VAL_INT) {
                int op_result = 0;

                if (strcmp(op, "+") == 0) {
                    op_result = left.int_val + right.int_val;
                } else if (strcmp(op, "-") == 0) {
                    op_result = left.int_val - right.int_val;
                } else if (strcmp(op, "*") == 0) {
                    op_result = left.int_val * right.int_val;
                } else if (strcmp(op, "/") == 0) {
                    if (right.int_val == 0) {
                        fprintf(stderr, "Runtime error: division by zero\n");
                        return (Value){ .kind = VAL_UNIT };
                    }
                    op_result = left.int_val / right.int_val;
                } else {
                    fprintf(stderr, "Runtime error: unknown operator '%s'\n", op);
                    return (Value){ .kind = VAL_UNIT };
                }

                result.kind = VAL_INT;
                result.int_val = op_result;
            } else if (left.kind == VAL_FLOAT && right.kind == VAL_FLOAT) {
                double op_result = 0.0;

                if (strcmp(op, "+.") == 0) {
                    op_result = left.float_val + right.float_val;
                } else if (strcmp(op, "-.") == 0) {
                    op_result = left.float_val - right.float_val;
                } else if (strcmp(op, "*.") == 0) {
                    op_result = left.float_val * right.float_val;
                } else if (strcmp(op, "/.") == 0) {
                    if (right.float_val == 0.0f) {
                        fprintf(stderr, "Runtime error: division by zero\n");
                        return (Value){ .kind = VAL_UNIT };
                    }
                    op_result = left.float_val / right.float_val;
                } else {
                    fprintf(stderr, "Runtime error: unknown operator '%s'\n", op);
                    return (Value){ .kind = VAL_UNIT };
                }

                result.kind = VAL_FLOAT;
                result.float_val = op_result;
            }

            break;
        }

        case NodeBlock: {
            result.kind = VAL_UNIT;
            for (int i = 0; i < node->block.count; i++) {
                ASTNode *stmt = node->block.statements[i];
                result = eval_ast(stmt);
            }
            break;
        }

        case NodePrint: {
            Value val = eval_ast(node->print.value);

            printf("- : %s = ", node->print.type);

            if (strcmp(node->print.type, "int") == 0 && val.kind == VAL_INT) {
                printf("%d\n", val.int_val);
            } else if (strcmp(node->print.type, "float") == 0 && val.kind == VAL_FLOAT) {
                printf("%lf\n", val.float_val);
            } else if (strcmp(node->print.type, "bool") == 0 && val.kind == VAL_BOOL) {
                printf("%s\n", val.bool_val ? "true" : "false");
            } else if (strcmp(node->print.type, "char") == 0 && val.kind == VAL_CHAR) {
                printf("%c\n", val.char_val);
            } else if (strcmp(node->print.type, "string") == 0 && val.kind == VAL_STRING) {
                printf("%s\n", val.string_val);
            } else {
                fprintf(stderr, "Runtime error: print type <%s> does not match evaluated value kind\n", node->print.type);
            }

            result.kind = VAL_UNIT;
            break;
        }

        default:
            fprintf(stderr, "Runtime error: unsupported node type %d\n", node->type);
            return (Value){ .kind = VAL_UNIT };
    }

    return result;
}
