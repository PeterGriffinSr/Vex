#include <stdlib.h>
#include "error.h"
#include "color.h"

extern int yylineno;
extern int yycolumn;
extern FILE *yyin;
extern char *yytext;
extern int yyleng;
extern const char *filename;

void print_full_line(FILE *file, int line_number) {
    char line[1024];
    int current_line = 1;

    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        if (current_line == line_number) {
            printf(BLUE "   |\n" WHITE);
            printf(MAGENTA " %d " BLUE "| " GRAY "   %s", line_number, line);
            break;
        }
        current_line++;
    }
}

void report_error(const char* message) {
    printf(LIGHT_RED "error" GRAY ": %s\n", message);    
    printf(BLUE "  --> " GRAY "%s:%d:%d\n", filename, yylineno, yycolumn);
    
    print_full_line(yyin, yylineno);

    printf(BLUE "\n   |");
    for (int i = 0; i < yycolumn + 2; i++) fputc(' ', stdout);
    fputs(LIGHT_RED " ^\n", stdout);


    printf(BLUE "   |\n");
    printf(GRAY "Compilation Failed. Exited at code: 1\n");
    exit(1);
}
