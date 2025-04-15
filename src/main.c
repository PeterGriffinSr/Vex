#ifdef _WIN32
// Silence deprecation warnings on Windows
#define _CRT_SECURE_NO_WARNINGS
#endif // _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/ast.h"
#include "include/memory.h"
#include "parser.h"

Arena *global_arena = NULL;
extern FILE *yyin;
extern ASTNode *root;
extern const char *filename;
extern void yylex_destroy(void);

int main(int argc, char *argv[]) {
    global_arena = arena_create(1024 * 1024);

    if (argc < 2) {
        fputs("vex: error: no input file\n", stderr);
        return EXIT_FAILURE;
    }

    filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "vex: error: could not read file '%s'\n", filename);
        return EXIT_FAILURE;
    }

    yyin = file;

    if (yyparse() == 0) {
        printAST(root, 0);
    } else {
        printf("Parsing failed.\n");
    }

    fclose(file);
    arena_destroy(global_arena);
    yylex_destroy();

    return EXIT_SUCCESS;
}
