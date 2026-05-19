#include <stdio.h>
#include <string.h>
#include "pla_io.h"

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
                fprintf(stderr, "Error: Invalid character '%c' in cube.\n", ch);
                free_cube(c);
                return NULL;
        }
    }

    c->pos_bits = p;
    c->neg_bits = n_b;
    return c;
}

void append_to_list(Cube** head, Cube** tail, Cube* new_cube) {
    if (*head == NULL) {
        *head = new_cube;
        *tail = new_cube;
    } else {
        (*tail)->next = new_cube;
        *tail = new_cube;
    }
}

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

bool read_pla_file(const char* filename, Cube** list_10, Cube** list_01) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return false;
    }

    int num_inputs = 0;
    int num_outputs = 0;
    int num_cubes = 0;
    Cube* tail_10 = NULL;
    Cube* tail_01 = NULL;
    char command[64];
    
    while (fscanf(fp, "%63s", command) != EOF) {
        if (strcmp(command, ".i") == 0) {
            fscanf(fp, "%d", &num_inputs);
        } else if (strcmp(command, ".o") == 0) {
            fscanf(fp, "%d", &num_outputs);
        } else if (strcmp(command, ".p") == 0) {
            fscanf(fp, "%d", &num_cubes);
            break;
        } else {
            char skip[256];
            fgets(skip, sizeof(skip), fp);
        }
    }

    if (num_inputs < 1 || num_inputs > 64) {
        fprintf(stderr, "Error: Invalid number of inputs (%d).\n", num_inputs);
        fclose(fp);
        return false;
    }

    char input_part[128];
    char output_part[128];

    for (int i = 0; i < num_cubes; i++) {
        if (fscanf(fp, "%127s %127s", input_part, output_part) != 2) {
            fprintf(stderr, "Warning: Reached EOF early.\n");
            break;
        }

        Cube* new_cube = parse_cube_string(input_part, num_inputs);
        if (!new_cube) {
            free_cube_list(*list_10);
            free_cube_list(*list_01);
            fclose(fp);
            return false;
        }

        if (strcmp(output_part, "10") == 0) {
            append_to_list(list_10, &tail_10, new_cube);
        } else if (strcmp(output_part, "01") == 0) {
            append_to_list(list_01, &tail_01, new_cube);
        } else {
            free_cube(new_cube);
        }
    }

    if (fscanf(fp, "%63s", command) != EOF) {
        // .e の確認など（必要に応じて）
    }

    fclose(fp);
    return true;
}

bool write_pla_file(const char* filename, int num_inputs, int num_outputs, const Cube* list_10, const Cube* list_01) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing.\n", filename);
        return false;
    }

    // ループ用のポインタ変数を、あらかじめ関数の先頭でしっかりと宣言します
    const Cube* curr = NULL;
    int i = 0;

    // 1. まず項数（Cubeの個数）を合計する
    int num_cubes = 0;
    for (curr = list_10; curr != NULL; curr = curr->next) num_cubes++;
    for (curr = list_01; curr != NULL; curr = curr->next) num_cubes++;

    // 2. ヘッダー情報の書き込み
    fprintf(fp, ".i %d\n", num_inputs);
    fprintf(fp, ".o %d\n", num_outputs);
    fprintf(fp, ".p %d\n", num_cubes);

    // 3. 10のリストの書き込み
    for (curr = list_10; curr != NULL; curr = curr->next) {
        for (i = 0; i < num_inputs; i++) {
            uint64_t mask = (1ULL << (num_inputs - 1 - i));
            
            // 外側で宣言した curr を使うので、絶対に「宣言されていない」エラーは起きません
            bool has_pos = (curr->pos_bits & mask) != 0;
            bool has_neg = (curr->neg_bits & mask) != 0;

            if (has_pos && !has_neg)      fputc('1', fp);
            else if (!has_pos && has_neg) fputc('0', fp);
            else                          fputc('-', fp);
        }
        fprintf(fp, " 10\n");
    }

    // 4. 01のリストの書き込み
    for (curr = list_01; curr != NULL; curr = curr->next) {
        for (i = 0; i < num_inputs; i++) {
            uint64_t mask = (1ULL << (num_inputs - 1 - i));
            
            bool has_pos = (curr->pos_bits & mask) != 0;
            bool has_neg = (curr->neg_bits & mask) != 0;

            if (has_pos && !has_neg)      fputc('1', fp);
            else if (!has_pos && has_neg) fputc('0', fp);
            else                          fputc('-', fp);
        }
        fprintf(fp, " 01\n");
    }

    // 5. フッターの書き込み
    fprintf(fp, ".e\n");

    fclose(fp);
    return true;
}