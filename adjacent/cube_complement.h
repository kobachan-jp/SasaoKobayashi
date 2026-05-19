#ifndef CUBE_COMPLEMENT_H
#define CUBE_COMPLEMENT_H

#include "cube_pool.h"

// 外部から「否定」を実行するときに呼び出すメインの関数
Cube* invert_logic_function(Cube* G, int n);

// 再帰用の内部関数（他のファイルでもし使う場合のために一応公開）
Cube* compute_complement_rec(Cube* G, const Cube* c_accum, int n);
Cube* invert_single_cube(const Cube* c, int n);
Cube* intersect_two_cubes(const Cube* a, const Cube* b);
Cube* append_lists_destructive(Cube* list1, Cube* list2);
int find_best_pivot_bit(const Cube* G, int n);

#endif