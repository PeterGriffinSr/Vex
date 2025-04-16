#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#if defined (_WIN32)  || defined(_WIN64)
    #include <windows.h>
#else
    #include <sys/utsname.h>
#endif
 
void printHelpMenu(void) {
    puts("Usage: vex [options] file...\n"
        "Options:\n"
        " --help           Displays this information.\n"
        " --help={optimizers|warnings|target}[,...].\n\n"
        " --version        Display compiler version information.\n\n"
        " -save-temps      Do not delete intermediate files.\n\n"
        " -S               Compile only; do not assemble or link.\n"
        " -c               Compile and assemble, but do not link.\n"
        " -o <file>        Place the output into <file>.\n\n"
        "Report bugs at <https://github.com/PeterGriffinSr/Vex/issues>");
}

void printOptimizersHelp(void) {
    puts("The following options control optimizations:\n"
            " -O<number>        Set optimization level to <number>\n");
}
 
void printTargetHelp(void) {
    puts("The following options are target specific:\n");
}
 
void printWarningsHelp(void) {
    puts("The following options control compiler warning messages:\n");
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
    if (strncmp(arg, "--help=", 7) == 0) {
        const char *topic = arg + 7;
        if (strcmp(topic, "optimizers") == 0) {
            printOptimizersHelp();
        } else if (strcmp(topic, "target") == 0) {
            printTargetHelp();
        } else if (strcmp(topic, "warnings") == 0) {
            printWarningsHelp();
        } else {
            fprintf(stderr, "unrecognized argument to '--help=' option: '%s'\n", topic);
            return true;
        }
        return true;
    }
    return false;
}

