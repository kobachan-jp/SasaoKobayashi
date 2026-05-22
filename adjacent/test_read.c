#include <stdio.h>
#include <stdlib.h>
#include "cube_pool.h" 
#include "pla_io.h"    

int main(void) {
    // 1. リスト初期化
    init_cube_pool();

    Cube* output_10_list = NULL;
    Cube* output_01_list = NULL;
    const char* filename = "test.pla";
    int input_num = 0;

    // 2. ファイル読み込み
    printf("Reading PLA file: %s ...\n", filename);
    if (read_pla_file(filename, &output_10_list, &output_01_list, &input_num)) {
        printf("Successfully loaded PLA file!\n");
        //10と01のリストがきちんと読み込まれているか確認.
        print_cube_list("Output 10 Group", output_10_list);
        print_cube_list("Output 01 Group", output_01_list);
    }else{
        printf("Cannot Read File\n");
        exit(0);
    }

    return 0;

}