#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tc.h"
#include "ast.h"
#include "repl.h"
#include "eval.h"
#include "parser.h"
#include "memory.h"

extern FILE *yyin;
extern ASTNode *root;
extern Arena *global_arena;
extern void yylex_destroy(void);

void vex_repl(void) {
    char line[1024];
    global_arena = arena_create(1024 * 1024);

    puts("Vex REPL\nType :quit to exit.\n");

    while (true) {
        printf(">>> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (strncmp(line, ":quit", 5) == 0) break;

        FILE* buffer = fmemopen(line, strlen(line), "r");
        if (!buffer) {
            fprintf(stderr, "Failed to open memory stream.\n");
            continue;
        }

        yyin = buffer;
        root = NULL;

        yyparse();
        typecheck(root);
        eval_ast(root);

        fclose(buffer);
    }

    arena_destroy(global_arena);
    yylex_destroy();
}
