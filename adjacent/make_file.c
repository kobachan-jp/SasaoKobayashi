#include "cube_complement.h"
#include "cube_pool.h"
#include "cube_restrict.h"
#include "filterfile.h"
#include "pla_io.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // 1. リスト初期化
  init_cube_pool();

  Cube *output_10_list = NULL;
  Cube *output_01_list = NULL;
  const char *filename = argv[1];
  int input_num = 0;

  // 2. ファイル読み込み
  // 入力ファイルを確認
  const char *clean_fp = trim_newline(filename);
  if (clean_fp == NULL) {
    fprintf(stderr, "Cannot complete trim_newline\n");
    return 0;
  }
  //.iとかいらないやつを飛ばして保存.
  const char *skip_fp = skip_directives(clean_fp);
  if (skip_fp == NULL) {
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
  save_cube_list("cube_10_pos.txt", &output_10_list, input_num, true);
  save_cube_list("cube_10_neg.txt", &output_10_list, input_num, false);
  save_cube_list("cube_01_pos.txt", &output_01_list, input_num, true);
  save_cube_list("cube_01_neg.txt", &output_01_list, input_num, false);
  // 3.制限を求める（9.4.1)
  Cube *uni = create_universe_cube();
  // Cube* uni = NULL;
  // uni = parse_cube_string("---1",4);
  Cube *F_or_R = create_union_F_or_R(output_10_list, output_01_list);
  save_cube_list("F_or_R_pos.txt", &F_or_R, input_num, true);
  save_cube_list("F_or_R_neg.txt", &F_or_R, input_num, false);
  Cube *G = compute_restriction_optimized(F_or_R, uni);
  save_cube_list("G_pos.txt", &G, input_num, true);
  save_cube_list("G_neg.txt", &G, input_num, false);
  // 制限まで確認完了
  // 4.Gの否定を求める(9.4.2)
  Cube *not_G = complement(G, input_num);
  save_cube_list("G_comp_pos.txt", &not_G, input_num, true);
  // 5.c&not_Gをとる(9.4.3)
  Cube *disjoint = intersect_list_and_cube(not_G, uni);
  save_cube_list("disjoint_pos.txt", &disjoint, input_num, true);
  save_cube_list("disjoint_neg.txt", &disjoint, input_num, false);
  // 3. 後片付け
  free_cube_list(output_10_list);
  free_cube_list(output_01_list);

  free_cube_list(uni);
  free_cube_list(G);
  free_cube_list(not_G);
  free_cube_list(disjoint);

  return 0;
}
/*



}

*/