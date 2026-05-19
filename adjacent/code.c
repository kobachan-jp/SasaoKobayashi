#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_CUBES 100000 

typedef struct Cube {
    uint64_t pos_bits;     
    uint64_t neg_bits;     
    struct Cube* next;     
} Cube;

static Cube cube_pool[MAX_CUBES];
static Cube* free_list_head = NULL;

void init_cube_pool() {
    free_list_head = &cube_pool[0];
    for (int i = 0; i < MAX_CUBES - 1; i++) {
        cube_pool[i].next = &cube_pool[i + 1];
    }
    cube_pool[MAX_CUBES - 1].next = NULL;
}

Cube* alloc_cube() {
    if (free_list_head == NULL) {
        fprintf(stderr, "Error: Cube pool is full.\n");
        return NULL;
    }
    Cube* allocated = free_list_head;
    free_list_head = free_list_head->next;
    allocated->next = NULL;
    return allocated;
}

void free_cube(Cube* cube) {
    if (cube == NULL) return;
    cube->next = free_list_head;
    free_list_head = cube;
}

void free_cube_list(Cube* head) {
    while (head != NULL) {
        Cube* next = head->next;
        free_cube(head);
        head = next;
    }
}

/**
 * 最高の出来の「左シフトパース関数」
 * 文字列から下位 n ビット分を読み込んでCubeを生成
 */
Cube* parse_cube_string(const char* cube_str, int n) {
    Cube* c = alloc_cube();
    if (!c) return NULL;

    uint64_t p = ~0ULL;
    uint64_t n_b = ~0ULL;

    for (int i = 0; i < n; i++) {
        p <<= 1;
        n_b <<= 1;

        char ch = cube_str[i];
        switch (ch) {
            case '1': p |= 1ULL;   break;
            case '0': n_b |= 1ULL; break;
            case '-': p |= 1ULL; n_b |= 1ULL; break;
            default:
                // 万が一不正な文字が混ざっていた場合はエラーを返してプールに戻す
                fprintf(stderr, "Error: Invalid character '%c' in cube.\n", ch);
                free_cube(c);
                return NULL;
        }
    }

    c->pos_bits = p;
    c->neg_bits = n_b;
    return c;
}

// リストの末尾に安全に追加するためのヘルパー関数
void append_to_list(Cube** head, Cube** tail, Cube* new_cube) {
    if (*head == NULL) {
        *head = new_cube;
        *tail = new_cube;
    } else {
        (*tail)->next = new_cube;
        *tail = new_cube;
    }
}

// デバッグ用：リストの中身を表示
void print_cube_list(const char* list_name, const Cube* head) {
    printf("\n--- List: %s ---\n", list_name);
    const Cube* current = head;
    int index = 1;
    while (current != NULL) {
        printf("  Cube %d: pos = 0x%016lx, neg = 0x%016lx\n", 
               index++, current->pos_bits, current->neg_bits);
        current = current->next;
    }
}

/**
 * PLAファイルを読み込み、出力に応じて2つのリストを構築する関数
 */
bool read_pla_file(const char* filename, Cube** list_10, Cube** list_01) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return false;
    }

    int num_inputs = 0;
    int num_outputs = 0;
    int num_cubes = 0;

    // リストの末尾追跡用ポインタ
    Cube* tail_10 = NULL;
    Cube* tail_01 = NULL;

    char command[64];
    
    // 1. ヘッダー情報の解析ループ
    while (fscanf(fp, "%63s", command) != EOF) {
        if (strcmp(command, ".i") == 0) {
            fscanf(fp, "%d", &num_inputs);
        } else if (strcmp(command, ".o") == 0) {
            fscanf(fp, "%d", &num_outputs);
        } else if (strcmp(command, ".p") == 0) {
            fscanf(fp, "%d", &num_cubes);
            break; // .p が読めたら、次からCubeデータ行が始まるのでループを抜ける
        } else {
            // 未知のコマンド、またはコメント等は読み飛ばす
            char skip[256];
            fgets(skip, sizeof(skip), fp);
        }
    }

    // 変数個数のバリデーション (1 <= n <= 64)
    if (num_inputs < 1 || num_inputs > 64) {
        fprintf(stderr, "Error: Invalid number of inputs (%d). Must be between 1 and 64.\n", num_inputs);
        fclose(fp);
        return false;
    }

    // 2. Cubeデータ行の読み込みループ
    // 1行最大64文字+αに対応できるように、少し余裕を持ったバッファを確保
    char input_part[128];
    char output_part[128];

    for (int i = 0; i < num_cubes; i++) {
        // 入力部分と出力部分を個別に fscanf で読み込む（空白を自動で飛ばしてくれます）
        if (fscanf(fp, "%127s %127s", input_part, output_part) != 2) {
            fprintf(stderr, "Warning: Reached EOF or bad format before reading %d cubes.\n", num_cubes);
            break;
        }

        // 最高の左シフトパース関数でCubeオブジェクトを生成
        Cube* new_cube = parse_cube_string(input_part, num_inputs);
        if (!new_cube) {
            // パースエラー時は、それまでに作ったリストを全解放して失敗とする
            free_cube_list(*list_10);
            free_cube_list(*list_01);
            fclose(fp);
            return false;
        }

        // 出力部の文字列に応じて、リストを振り分ける
        if (strcmp(output_part, "10") == 0) {
            append_to_list(list_10, &tail_10, new_cube);
        } else if (strcmp(output_part, "01") == 0) {
            append_to_list(list_01, &tail_01, new_cube);
        } else {
            // 指定以外の出力（noなど）は無視してプールに返却
            free_cube(new_cube);
        }
    }

    // 3. 最後にフッター ".e" を確認して終了
    if (fscanf(fp, "%63s", command) != EOF) {
        if (strcmp(command, ".e") != 0) {
            // エラーではないですが、フォーマット確認用
            // fprintf(stderr, "Note: File did not end with .e cleanly\n");
        }
    }

    fclose(fp);
    return true;
}

int main() {
    init_cube_pool();

    // 2つの独立したリストの先頭ポインタ
    Cube* output_10_list = NULL;
    Cube* output_01_list = NULL;

    // テスト用のファイル名（例として "test.pla" とします）
    // ※ 実際に動かす際は、このファイルを作成するか適切なパスに変更してください。
    const char* filename = "test.pla";

    printf("Reading PLA file: %s ...\n", filename);
    if (read_pla_file(filename, &output_10_list, &output_01_list)) {
        printf("Successfully loaded PLA file!\n");

        // 各リストの結果をダンプ（デバッグ用）
        print_cube_list("Output 10 Group", output_10_list);
        print_cube_list("Output 01 Group", output_01_list);
    }

    // 後片付け（プログラム終了時に一括返却）
    free_cube_list(output_10_list);
    free_cube_list(output_01_list);

    return 0;
}