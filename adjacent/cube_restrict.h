#ifndef CUBE_RESTRICT_H
#define CUBE_RESTRICT_H

#include "cube_pool.h" 


Cube* duplicate_list(const Cube* head);
Cube* create_union_F_or_R(const Cube* list_10, const Cube* list_01);
Cube* compute_restriction_optimized(const Cube* list_10, const Cube* list_01);
Cube* create_universe_cube(void);

#endif