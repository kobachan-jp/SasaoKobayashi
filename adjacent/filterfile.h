#ifndef FILTER_FILE_H
#define FILTER_FILE_H

#include <stdio.h>

void trim_newline(const char* filename);
void skip_directives(FILE *fp);
#endif