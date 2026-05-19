#ifndef CUBE_RESTRICT_H
#define CUBE_RESTRICT_H

#include "cube_pool.h" 


Cube* duplicate_list(const Cube* head);
Cube* create_union_F(const Cube* list_10, const Cube* list_01);
Cube* intersect_cube_with_universe(const Cube* c, const Cube* uni);
Cube* compute_restriction_optimized(const Cube* list_10, const Cube* list_01, const Cube* universe);

#endif