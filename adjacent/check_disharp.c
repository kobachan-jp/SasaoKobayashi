#include <stdio.h>
#include "cube_pool.h" 
#include "filterfile.h"
#include "pla_io.h"    
#include "cube_restrict.h"
#include "cube_complement.h"


int main(int argc, char *argv[]) {
    // 1. リスト初期化
    init_cube_pool();

    Cube* F_list = NULL;
    const char* filename = argv[1];
    int input_num = 0;

    // 2. ファイル読み込み
        //入力ファイルを確認
        const char *clean_fp = trim_newline(filename);
        if(clean_fp == NULL){
            fprintf(stderr,"Cannot complete trim_newline\n");
            return 0;
        }
        //.iとかいらないやつを飛ばして保存.
        const char *skip_fp = skip_directives(clean_fp);
        if(skip_fp == NULL){
            fprintf(stderr,"Cannot complete skip_directives\n");
            return 0;
        }
        make_cube_list(clean_fp,&F_list,&input_num);
        fprintf_cube_list_combined("F_list.txt",F_list, input_num);
        //3.制限を求める（9.4.1)
        Cube* H_list = NULL;
        const char *filename2 = argv[2];
         const char *clean_fp2 = trim_newline(filename2);
        if(clean_fp2 == NULL){
            fprintf(stderr,"Cannot complete trim_newline\n");
            return 0;
        }
        //.iとかいらないやつを飛ばして保存.
        const char *skip_fp2 = skip_directives(clean_fp2);
        if(skip_fp2 == NULL){
            fprintf(stderr,"Cannot complete skip_directives\n");
            return 0;
        }
        make_cube_list(clean_fp2,&H_list,&input_num);
        fprintf_cube_list_combined("H_list.txt",H_list, input_num);
        Cube* sum = NULL;

        int idx = 0;
for (Cube* h = H_list; h != NULL; h = h->next) {
    printf("\n===== H[%d] =====\n", idx);
    printf("h:\n");
    fprintf_bits(stderr, h->pos_bits, input_num);
    fprintf_bits(stderr, h->neg_bits, input_num);
    Cube* G = compute_restriction_optimized(F_list, h);
    printf("F count=%d\n", count_cubes(F_list));
     printf("G count = %d\n", count_cubes(G));
    Cube* not_G = complement(G, input_num);
  printf("not_G count = %d\n", count_cubes(not_G));     
    Cube* disjoint = intersect_list_and_cube(not_G, h);
printf("disjoint count = %d\n", count_cubes(disjoint));
    sum = append_lists_destructive(sum, disjoint);

    free_cube_list(G);
    free_cube_list(not_G);
    /* disjoint は sum に連結したので解放しない */
}

fprintf_cube_list_combined("disjoint_H&F.txt", sum, input_num);

/* 後片付け */
free_cube_list(F_list);
free_cube_list(H_list);
free_cube_list(sum);
fprintf_cube_list_combined("disjoint_H&F.txt",sum,input_num);
    // 3. 後片付け
    free_cube_list(F_list);
    free_cube_list(H_list);
    free_cube_list(sum);  

    return 0;
            }
    /*
        
    
    
}

*/