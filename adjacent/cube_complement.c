#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include<stdbool.h>
#include "cube_complement.h"
#include "cube_restrict.h"
#include "cube_pool.h"
#include "pla_io.h"
/*
共通bitがあるcubeを探す
*/
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
//ただcubeセットをくっつけているだけ
Cube* append_lists_destructive(Cube* list1, Cube* list2) {
    if (!list1) return list2;
    if (!list2) return list1;
    Cube* tail = list1;
    while (tail->next != NULL) tail = tail->next;
    tail->next = list2;
    return list1;
}

/**
 * 集合F（Cubeのリスト）と、1つのCube c のAND（積集合）を計算し、新しいリストを返す
 */
Cube* intersect_list_and_cube(const Cube* F, const Cube* c) {
    if (F == NULL || c == NULL) return NULL;

    Cube* res_head = NULL;
    Cube* res_tail = NULL;
    const Cube* curr_F = F;

    while (curr_F != NULL) {
        // 💡 1. 集合Fの今の項と、Cube c のAND（ビット演算）をとる
        uint64_t p = curr_F->pos_bits & c->pos_bits;
        uint64_t n_b = curr_F->neg_bits & c->neg_bits;

        // 💡 2. 矛盾チェック（posもnegも0のビットがあれば、ANDをとると消滅する）
        if ((~p & ~n_b) != 0ULL) {
            curr_F = curr_F->next;
            continue; // 矛盾した項は飛ばす（リストに入れない）
        }

        // 💡 3. 矛盾しなければ、ANDがとれた新しいCubeをallocしてリストに追加
        Cube* new_cube = alloc_cube();
        if (new_cube != NULL) {
            new_cube->pos_bits = p;
            new_cube->neg_bits = n_b;
            new_cube->next = NULL;

            if (res_head == NULL) {
                res_head = new_cube;
                res_tail = new_cube;
            } else {
                res_tail->next = new_cube;
                res_tail = new_cube;
            }
        }
        curr_F = curr_F->next;
    }

    return res_head;
}

//activeなbit探し
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

//activeな列を1としたcubeを作成
Cube* create_active_on_bit(int n, int pivot){
    Cube* c1 = alloc_cube();
    uint64_t mask = (1ULL << (n - 1 - pivot));
    c1->pos_bits = ~0ULL;
    c1 -> neg_bits = ~0ULL & ~mask;
    c1 ->next = NULL; 
    return c1;
}

//activeな列を0としたcubeを作成
Cube* create_active_off_bit(int n, int pivot){

    uint64_t mask = (1ULL << (n - 1 - pivot));
    Cube* c2 = alloc_cube();
    c2 -> pos_bits = ~0ULL & ~mask;
    c2 -> neg_bits = ~0ULL;
    c2 -> next = NULL; 

    return c2;
}

Cube* complement(Cube* G, int n){
    //ex.9.10.1
    int pivot = find_best_pivot_bit(G, n);
    printf("best_pivot : %d\n",pivot);
    if(pivot == -1){
        return invert_single_cube(G,n);
    }
    Cube* c1 = create_active_on_bit(n, pivot);
    printf("active on bits\n");
    fprintf_bits(stderr,c1->pos_bits,n);
    Cube* c2 = create_active_off_bit(n, pivot);
    printf("active off bits\n");
    fprintf_bits(stderr,c2->pos_bits,n);
    Cube* G1 = compute_restriction_optimized(G, c1);
    Cube* G2 = compute_restriction_optimized(G, c2);
    //ex.9.10.2
    int n1 = count_cubes(G1);
    int n2 = count_cubes(G2);
    Cube* not_G1 = NULL;
    Cube* not_G2 = NULL;
    //ここから再帰するぶぶんスタート
    if(n1 == 1){
        not_G1 = invert_single_cube(G1,n);
        printf("Before NOT:\n");
        fprintf_bits(stderr,G1->pos_bits,n);
        printf("after NOT:\n");
        fprintf_bits(stderr,not_G1->pos_bits,n);
    }else{
        not_G1 = complement(G1,n);
    }
    if(n2 == 1){
        not_G2 = invert_single_cube(G2,n);
        printf("Before NOT:\n");
        fprintf_bits(stderr,G2->pos_bits,n);
        printf("after NOT:\n");
        fprintf_bits(stderr,not_G2->pos_bits,n);
    }else{
        not_G2 = complement(G2,n);
    }

    Cube* final_res1 = intersect_list_and_cube(not_G1, c1);
    Cube* final_res2 = intersect_list_and_cube(not_G2, c2);
    
    free_cube_list(G1);
    free_cube_list(G2);
    free_cube_list(not_G1);
    free_cube_list(not_G2);
    
    return append_lists_destructive(final_res1, final_res2);
}


//制限で求めたFの否定を求める
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



