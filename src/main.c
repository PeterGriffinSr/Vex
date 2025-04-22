#ifdef _WIN32
// Silence deprecation warnings on Windows
#define _CRT_SECURE_NO_WARNINGS
#endif // _WIN32

#include <stdlib.h>
#include "common.h"
#include "parser.h"
#include "memory.h"
#include "llvm.h"
#include "tc.h"

Arena *global_arena = NULL;
extern FILE *yyin;
extern ASTNode *root;
extern const char *filename;
extern void yylex_destroy(void);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fputs("vex: error: no input file\n", stderr);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (handleCliOption(argv[i])) {
            return EXIT_SUCCESS;
        }
    }

    global_arena = arena_create(1024 * 1024);

    filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "vex: error: could not read file '%s'\n", filename);
        return EXIT_FAILURE;
    }

    yyin = file;
    root = NULL;

    if (yyparse() == 0) {
        printAST(root, 0);
        arena_new_line();
    } else {
        printf("Parsing failed.\n");
    }
    typecheck(root);

    init_llvm_codegen();
    compile_root();
    write_llvm_ir_to_file("output.ll");
    print_llvm_ir();

    fclose(file);
    arena_destroy(global_arena);
    yylex_destroy();
    LLVMDisposeBuilder(Builder);
    LLVMDisposeModule(TheModule);
    LLVMContextDispose(TheContext);
    LLVMShutdown();

    return EXIT_SUCCESS;
}
