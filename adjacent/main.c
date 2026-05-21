#include <stdio.h>
#include "cube_pool.h" 
#include "pla_io.h"    
#include "cube_restrict.h"
#include "cube_complement.h"


int main(void) {
    // 1. リスト初期化
    init_cube_pool();

    Cube* output_10_list = NULL;
    Cube* output_01_list = NULL;
    const char* filename = "test.pla";

    // 2. ファイル読み込み
    printf("Reading PLA file: %s ...\n", filename);
    if (read_pla_file(filename, &output_10_list, &output_01_list)) {
        printf("Successfully loaded PLA file!\n");
        //10と01のリストがきちんと読み込まれているか確認.
        print_cube_list("Output 10 Group", output_10_list);
        print_cube_list("Output 01 Group", output_01_list);
    }else{
        printf("Cannot Read File\n");
        exit(0);
    }

    //3.制限を求める（9.4.1)
    Cube* uni = create_universe_cube();
    Cube* F_or_R = create_union_F_or_R(output_10_list,output_01_list);
    Cube* G = compute_restriction_optimized(F_or_R, uni);
    //4.Gの否定を求める(9.4.2)
    
    // 3. 後片付け
    free_cube_list(output_10_list);
    free_cube_list(output_01_list);

    return 0;
}