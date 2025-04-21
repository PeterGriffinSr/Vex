#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "repl.h"

#if defined (_WIN32)  || defined(_WIN64)
    #include <windows.h>
#else
    #include <sys/utsname.h>
#endif
 
void printHelpMenu(void) {
    puts("Usage: vex [options] file...\n"
         "Options:\n"
         "  --help                   Display this information.\n"
         "  --help={optimizers|warnings|target|compiler}[,...]\n"
         "                           Display help on specific option categories.\n"
         "  --version                Display compiler version information.\n\n"
         "  repl                     Launch the interactive Vex REPL (Read-Eval-Print Loop).\n\n"
         "Report bugs at <https://github.com/PeterGriffinSr/Vex/issues>");
}

void printOptimizersHelp(void) {
    puts("Optimization Options:\n"
         "  -O0                      Disable all optimizations (default).\n"
         "  -O1                      Enable basic optimizations.\n"
         "  -O2                      Enable additional optimizations.\n"
         "  -O3                      Enable full optimizations, including inlining.\n"
         "  -Os                      Optimize for size.\n"
         "  -Ofast                   Enable aggressive optimizations that may break strict standards compliance.\n");
}

void printTargetHelp(void) {
    puts("Target-Specific Options:\n"
         "  --target=<platform>     Specify the target platform (e.g., linux, wasm, arm).\n"
         "  --arch=<arch>           Specify the target architecture (e.g., x86_64, arm64).\n"
         "  --emit-llvm             Output LLVM IR instead of native code.\n");
}

void printWarningsHelp(void) {
    puts("Warning Control Options:\n"
         "  -Wall                   Enable most warnings.\n"
         "  -Werror                 Treat warnings as errors.\n"
         "  -Wno-unused             Disable warnings for unused variables or functions.\n"
         "  -Wextra                 Enable extra warning checks.\n");
}

void printCompilerHelp(void) {
    puts("Compiler Control Options:\n"
         "  -save-temps             Do not delete intermediate files (e.g., .ll, .s).\n"
         "  -S                      Compile only; do not assemble or link.\n"
         "  -c                      Compile and assemble, but do not link.\n"
         "  -o <file>               Place the output into <file>.\n"
         "  --emit-ast              Output the parsed AST instead of compiling.\n"
         "  --emit-ir               Output the intermediate representation (IR).\n");
}

void printVersion(void) {
    char os_name[128];
    systemInfo(os_name, sizeof(os_name));
    printf("vex version %d.%d.%d (%s %d.%d.%d)\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, os_name, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
}

void systemInfo(char *output, size_t size) {
#if defined (__unix__) || defined (__linux__) || defined (__APPLE__)
    struct utsname buffer;
#endif
 
    if (output == NULL || size == 0) {
        fputs("Invalid output buffer or size\n", stderr);
        return;
    }
 
#if defined(_WIN32) || defined(_WIN64)
    snprintf(output, size, "%s","Windows");
#else
    if (uname(&buffer) != 0) {
        perror("uname");
        snprintf(output, size, "%s", "Unknown");
    } else {
        snprintf(output, size,"%s", buffer.sysname); 
    }
#endif
    output[size - 1] = '\0'; 
}

bool handleCliOption(const char *arg) {
    if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0) {
        printVersion();
        return true;
    }
    if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
        printHelpMenu();
        return true;
    }
    if (strcmp(arg, "repl") == 0) {
        vex_repl();
        return true;
    }
    if (strncmp(arg, "--help=", 7) == 0) {
        const char *topic = arg + 7;
        if (strcmp(topic, "optimizers") == 0) {
            printOptimizersHelp();
        } else if (strcmp(topic, "target") == 0) {
            printTargetHelp();
        } else if (strcmp(topic, "warnings") == 0) {
            printWarningsHelp();
        } else if (strcmp(topic, "compiler") == 0) {
            printCompilerHelp();
        } else {
            fprintf(stderr, "unrecognized argument to '--help=' option: '%s'\n", topic);
            return true;
        }
        return true;
    }
    return false;
}
