#ifndef CUBE_POOL_H
#define CUBE_POOL_H

#include <stdint.h>

#define MAX_CUBES 100000

typedef struct Cube {
    uint64_t pos_bits;     
    uint64_t neg_bits;     
    struct Cube* next;     
} Cube;

void init_cube_pool(void);
Cube* alloc_cube(void);
void free_cube(Cube* cube);
void free_cube_list(Cube* head);
int count_cubes(const Cube* head);
#endif