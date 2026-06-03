#ifndef PLA_IO_H
#define PLA_IO_H

#include <stdbool.h>
#include "cube_pool.h" 


Cube* parse_cube_string(const char* cube_str, int n);
void append_to_list(Cube** head, Cube** tail, Cube* new_cube);
void fprintf_bits(FILE* fp, unsigned long bits, int input_number);
void save_cube_list(const char* output_filename, Cube** cube_list, int input_number, bool n);
void make_cube_list(const char* filename, Cube** cube_list, int *input_num);
void fprintf_cube_combined(FILE* stream, const Cube* c, int n);
void fprintf_cube_list_combined(const char *stream, const Cube* head, int n);
#endif 