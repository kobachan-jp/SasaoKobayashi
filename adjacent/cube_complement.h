#ifndef CUBE_COMPLEMENT_H
#define CUBE_COMPLEMENT_H

#include "cube_pool.h"

Cube* complement(Cube* G, int n);
Cube* invert_single_cube(const Cube* c, int n);
Cube* intersect_two_cubes(const Cube* a, const Cube* b);
Cube* intersect_list_and_cube(const Cube* F, const Cube* c);
Cube* append_lists_destructive(Cube* list1, Cube* list2);
int find_best_pivot_bit(const Cube* G, int n);

#endif