#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include<stdbool.h>
#include "cube_complement.h"
#include "cube_restrict.h"
#include "cube_pool.h"

Cube* intersect_two_cubes(const Cube* a, const Cube* b) {
    uint64_t p = a->pos_bits & b->pos_bits;
    uint64_t n_b = a->neg_bits & b->neg_bits;
    
    if ((~p & ~n_b) != 0ULL) return NULL; // 矛盾
    
    Cube* res = alloc_cube();
    if (!res) return NULL;
    res->pos_bits = p;
    res->neg_bits = n_b;
    return res;
}

Cube* append_lists_destructive(Cube* list1, Cube* list2) {
    if (!list1) return list2;
    if (!list2) return list1;
    Cube* tail = list1;
    while (tail->next != NULL) tail = tail->next;
    tail->next = list2;
    return list1;
}

int find_best_pivot_bit(const Cube* G, int n) {
    if (!G) return -1;
    
    int best_bit = -1;
    int min_difference = 999999;

    for (int i = 0; i < n; i++) {
        uint64_t mask = (1ULL << (n - 1 - i));
        int count_0 = 0;
        int count_1 = 0;
        
        const Cube* curr = G;
        while (curr != NULL) {
            bool has_pos = (curr->pos_bits & mask) != 0;
            bool has_neg = (curr->neg_bits & mask) != 0;
            
            if (has_pos && !has_neg) count_1++;      
            else if (!has_pos && has_neg) count_0++; 
            
            curr = curr->next;
        }
        
        if (count_0 == 0 && count_1 == 0) continue;
        
        int diff = (count_0 > count_1) ? (count_0 - count_1) : (count_1 - count_0);
        
        if (diff < min_difference) {
            min_difference = diff;
            best_bit = i;
        }
    }
    return best_bit;
}

Cube* invert_single_cube(const Cube* c, int n) {
    Cube* res_head = NULL;
    Cube* res_tail = NULL;
    
    for (int i = 0; i < n; i++) {
        uint64_t mask = (1ULL << (n - 1 - i));
        bool has_pos = (c->pos_bits & mask) != 0;
        bool has_neg = (c->neg_bits & mask) != 0;
        
        Cube* inverted = NULL;
        if (has_pos && !has_neg) {
            inverted = alloc_cube();
            if (inverted) {
                inverted->pos_bits = ~0ULL & ~mask;
                inverted->neg_bits = ~0ULL;
            }
        } else if (!has_pos && has_neg) {
            inverted = alloc_cube();
            if (inverted) {
                inverted->pos_bits = ~0ULL;
                inverted->neg_bits = ~0ULL & ~mask;
            }
        }
        
        if (inverted) {
            if (!res_head) { res_head = inverted; res_tail = inverted; }
            else { res_tail->next = inverted; res_tail = inverted; }
        }
    }
    return res_head;
}

Cube* compute_complement_rec(Cube* G, const Cube* c_accum, int n) {
    if (G == NULL) {
        Cube* res = alloc_cube();
        if (res) { res->pos_bits = c_accum->pos_bits; res->neg_bits = c_accum->neg_bits; }
        return res;
    }
    
    if (G->next == NULL) {
        Cube* G_not = invert_single_cube(G, n);
        Cube* final_res_head = NULL;
        Cube* final_res_tail = NULL;
        Cube* curr = G_not;
        
        while (curr != NULL) {
            Cube* intersected = intersect_two_cubes(c_accum, curr);
            if (intersected) {
                if (!final_res_head) { final_res_head = intersected; final_res_tail = intersected; }
                else { final_res_tail->next = intersected; final_res_tail = intersected; }
            }
            curr = curr->next;
        }
        free_cube_list(G_not);
        return final_res_head;
    }
    
    int pivot = find_best_pivot_bit(G, n);
    
    if (pivot == -1) {
        Cube* G_not = invert_single_cube(G, n);
        Cube* final_res_head = NULL; Cube* final_res_tail = NULL; Cube* curr = G_not;
        while (curr != NULL) {
            Cube* intersected = intersect_two_cubes(c_accum, curr);
            if (intersected) {
                if (!final_res_head) { final_res_head = intersected; final_res_tail = intersected; }
                else { final_res_tail->next = intersected; final_res_tail = intersected; }
            }
            curr = curr->next;
        }
        free_cube_list(G_not);
        return final_res_head;
    }
    
    uint64_t mask = (1ULL << (n - 1 - pivot));
    
    Cube c1 = { .pos_bits = ~0ULL, .neg_bits = ~0ULL & ~mask, .next = NULL }; 
    Cube c2 = { .pos_bits = ~0ULL & ~mask, .neg_bits = ~0ULL, .next = NULL }; 
    
    Cube* next_c1 = intersect_two_cubes(c_accum, &c1);
    Cube* next_c2 = intersect_two_cubes(c_accum, &c2);
    
    // 💡 ここで cube_restrict の関数を呼び出します！
    Cube* G1 = compute_restriction_optimized(G, NULL, &c1);
    Cube* G2 = compute_restriction_optimized(G, NULL, &c2);
    
    Cube* res1 = NULL;
    Cube* res2 = NULL;
    if (next_c1) res1 = compute_complement_rec(G1, next_c1, n);
    if (next_c2) res2 = compute_complement_rec(G2, next_c2, n);
    
    free_cube_list(G1);
    free_cube_list(G2);
    if (next_c1) free_cube(next_c1);
    if (next_c2) free_cube(next_c2);
    
    return append_lists_destructive(res1, res2);
}

Cube* invert_logic_function(Cube* G, int n) {
    Cube universe = { .pos_bits = ~0ULL, .neg_bits = ~0ULL, .next = NULL };
    return compute_complement_rec(G, &universe, n);
}