#ifndef PLA_IO_H
#define PLA_IO_H

#include <stdbool.h>
#include "cube_pool.h" 


Cube* parse_cube_string(const char* cube_str, int n);
void append_to_list(Cube** head, Cube** tail, Cube* new_cube);
void print_cube_list(const char* list_name, const Cube* head);
bool read_pla_file(const char* filename, Cube** list_10, Cube** list_01);
bool write_pla_file(const char* filename, int num_inputs, int num_outputs, const Cube* list_10, const Cube* list_01);
#endif 