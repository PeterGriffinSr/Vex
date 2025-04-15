#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

void print_full_line(FILE *file, int line_number);
void report_error(const char* message);

#endif // ERROR_H
