#include <stdio.h>
#include <stddef.h>
#include "cube_restrict.h" // 自身のヘッダー
#include "cube_pool.h"     // alloc_cube や free_cube_list を使うために必要

Cube* duplicate_list(const Cube* head) {
    Cube* new_head = NULL;
    Cube* new_tail = NULL;
    const Cube* curr = head;
    
    while (curr != NULL) {
        Cube* copy = alloc_cube();
        if (!copy) return NULL;
        copy->pos_bits = curr->pos_bits;
        copy->neg_bits = curr->neg_bits;
        copy->next = NULL;
        
        if (new_head == NULL) {
            new_head = copy;
            new_tail = copy;
        } else {
            new_tail->next = copy;
            new_tail = copy;
        }
        curr = curr->next;
    }
    return new_head;
}


Cube* create_union_F_or_R(const Cube* list_10, const Cube* list_01) {
    Cube* head_10 = duplicate_list(list_10);
    Cube* head_01 = duplicate_list(list_01);
    
    if (head_10 == NULL) return head_01;
    if (head_01 == NULL) return head_10;
    
    Cube* tail = head_10;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = head_01;
    
    return head_10;
}

/*
 * すべての変数が '-' (Don't Care) となる Universe Cube を生成して返す関数
*/
Cube* create_universe_cube(void) {
    Cube* uni = alloc_cube();
    if (!uni) {
        fprintf(stderr, "Error: Failed to allocate memory for Universe Cube.\n");
        return NULL;
    }

    uni->pos_bits = ~0ULL; // 0xffffffffffffffff
    uni->neg_bits = ~0ULL; // 0xffffffffffffffff
    uni->next = NULL;
    
    return uni;
}

Cube* compute_restriction_optimized(const Cube* F, const Cube* c) {
    
    Cube* G_head = NULL;
    Cube* G_tail = NULL;
    Cube* curr_F = F;
    

    uint64_t not_uni_pos = ~c->pos_bits;
    uint64_t not_uni_neg = ~c->neg_bits;
    
    while (curr_F != NULL) {
        uint64_t p = curr_F->pos_bits & c->pos_bits;
        uint64_t n_b = curr_F->neg_bits & c->neg_bits;
        
        if ((~p & ~n_b) != 0ULL) {
            curr_F = curr_F->next;
            continue; 
        }
        
        p |= not_uni_pos;
        n_b |= not_uni_neg;
        
        Cube* res_cube = alloc_cube();
        if (res_cube != NULL) {
            res_cube->pos_bits = p;
            res_cube->neg_bits = n_b;
            res_cube->next = NULL;
            
            if (G_head == NULL) {
                G_head = res_cube;
                G_tail = res_cube;
            } else {
                G_tail->next = res_cube;
                G_tail = res_cube;
            }
        }
        
        curr_F = curr_F->next;
    }
    
    free_cube_list(F);
    free_cube_list(c);
    return G_head;
}