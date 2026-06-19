#include "cube_pool.h"
#include "cube_restrict.h"
#include "filterfile.h"
#include "pla_io.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    // 1. リスト初期化
    init_cube_pool();

    Cube *output_10_list = NULL;
    Cube *output_01_list = NULL;
    const char *filename = argv[1];
    int input_num = 0;

    // 2. ファイル読み込み
    // 入力ファイルを確認
    const char *clean_fp = trim_newline(filename);
    if (clean_fp == NULL)
    {
        fprintf(stderr, "Cannot complete trim_newline\n");
        return 0;
    }
    //.iとかいらないやつを飛ばして保存.
    const char *skip_fp = skip_directives(clean_fp);
    if (skip_fp == NULL)
    {
        fprintf(stderr, "Cannot complete skip_directives\n");
        return 0;
    }
    // 10の入力部のみ保存.
    extract_input(skip_fp, "10", "output_10.txt");
    // 01の入力部のみ保存.
    extract_input(skip_fp, "01", "output_01.txt");
    // 10と01のリストをbitに変換(肯定と否定それぞれ分けて保存).
    make_cube_list("output_10.txt", &output_10_list, &input_num);
    make_cube_list("output_01.txt", &output_01_list, &input_num);
    // disjoint sharp演算をしてDCを求める.
    // 1.制限を求める（9.4.1)
    Cube *uni = create_universe_cube();
    Cube *F_or_R = create_union_F_or_R(output_10_list, output_01_list);
    fprintf_cube_list_combined("F_or_R.txt", F_or_R, input_num, "");
    free_cube_list(output_01_list);
    Cube *G = compute_restriction_optimized(F_or_R, uni);

    free_cube(uni);
    free_cube_list(G);
    printf("completed restrict\n");
    return 0;
}
/*





*/