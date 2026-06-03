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
        Cube* G = compute_restriction_optimized(F_list,H_list);
        fprintf_cube_list_combined("restrict_F&H.txt",G,input_num);
        //制限まで確認完了
        //4.Gの否定を求める(9.4.2)
        Cube* not_G = complement(G,input_num);
        //5.c&not_Gをとる(9.4.3)
        Cube* disjoint = intersect_list_and_cube(not_G,H_list);
        fprintf_cube_list_combined("disjoint_H&F.txt",disjoint,input_num);
    // 3. 後片付け
    free_cube_list(F_list);
    free_cube_list(H_list);
    free_cube_list(G);         
    free_cube_list(not_G);     
    free_cube_list(disjoint);  

    return 0;
            }
    /*
        
    
    
}

*/