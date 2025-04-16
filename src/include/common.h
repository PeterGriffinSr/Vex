#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include "ast.h"
#include "memory.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define PATCH_VERSION 0

void printHelpMenu(void);
void printVersion(void);
void printOptimizersHelp(void);
void printTargetHelp(void);
void printWarningsHelp(void);
void systemInfo(char *output, size_t size);
bool handleCliOption(const char *arg);

#endif // COMMON_H
