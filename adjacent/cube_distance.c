#include <stdio.h>
#include <stddef.h>
#include "cube_distance.h" // 自身のヘッダー
#include "cube_pool.h"     // alloc_cube や free_cube_list を使うために必要


/*Cube同士の距離を測る*/
int check_distance(Cube* a, Cube* b){
    if(a == NULL || b == NULL){
            return NULL;
        }
    uint64_t pos = a -> pos_bits & b -> pos_bits;
    uint64_t neg = a -> neg_bits & b -> neg_bits;

    int distance = __builtin_popcountll(~(pos | neg));

    return distance;
}
/*Cubeリスト同士でGの中でＦのcubeとの距離が1のCubeのリストを作成*/
Cube* make_distance1_CubeList(Cube* F, Cube* G){
    if(F == NULL || G == NULL){
        fprintf(stderr,"Cannot make distance1 CubeList./n");
        return NULL;
    }

    Cube* head = NULL;
    Cube* tail = NULL;
    for(Cube* a = F; a -> next != NULL; a = a->next){
        for (Cube* b = G; b -> next != NULL; b = b -> next)
        {   int count = check_distance(a,b);
            if(count != 1){
                continue;
            }
        Cube* new_cube = alloc_cube();
        if (new_cube != NULL) {
            new_cube->pos_bits = b -> pos_bits;
            new_cube->neg_bits = b -> neg_bits;
            new_cube->next = NULL;

            if (head == NULL) {
                head = new_cube;
                tail = new_cube;
            } else {
                tail->next = new_cube;
                tail = new_cube;
            }
        }
        }
        
    }
    return head;
}
