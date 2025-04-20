#ifndef EVAL_H
#define EVAL_H

#include <stdbool.h>
#include "ast.h"

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_CHAR,
    VAL_STRING,
    VAL_UNIT
} ValueKind;

typedef struct {
    ValueKind kind;
    union {
        int int_val;
        double float_val;
        int bool_val;
        char char_val;
        const char *string_val;
    };
} Value;

Value eval_ast(ASTNode *node);

#endif
