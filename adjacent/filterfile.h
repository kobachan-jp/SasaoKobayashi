#ifndef FILTER_FILE_H
#define FILTER_FILE_H

#include <stdio.h>

const char* trim_newline(const char* filename);
const char* skip_directives(const char* fp);
void extract_input(const char* fp, char* output_number, const char* save_filename);
#endif