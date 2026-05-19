#include <stdio.h>
#include "cube_pool.h" 
#include "pla_io.h"    

int main(void) {
    // 1. システム（プール）の初期化
    init_cube_pool();

    Cube* output_10_list = NULL;
    Cube* output_01_list = NULL;
    const char* filename = "test.pla";

    // 2. 処理の実行
    printf("Reading PLA file: %s ...\n", filename);
    if (read_pla_file(filename, &output_10_list, &output_01_list)) {
        printf("Successfully loaded PLA file!\n");

        print_cube_list("Output 10 Group", output_10_list);
        print_cube_list("Output 01 Group", output_01_list);
        
        
    }

    // 3. 後片付け
    free_cube_list(output_10_list);
    free_cube_list(output_01_list);

    return 0;
}