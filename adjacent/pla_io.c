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

// 64ビットのデータを、0と1のビット文字列としてファイルに書き出す関数
void fprintf_bits(FILE* fp, unsigned long bits, int input_number) {
    // 最上位ビット（左端）から1ビットずつチェックして '0' か '1' を書き込む
    for (int i = input_number-1; i >= 0; i--) {
        int bit = (bits >> i) & 1;
        fprintf(fp, "%d", bit);
    }
    fprintf(fp, "\n");
}

void save_cube_list(const char* output_filename, Cube** cube_list, int input_number, bool n){
    FILE *fp = fopen(output_filename,"w");
    if(fp==NULL){
        fprintf(stderr,"Cannot Open save_cube list output_file\n");
        return;
    }
    Cube *head = *cube_list;
    while(head != NULL){
        if(n == true){
        fprintf_bits(fp,head->pos_bits,input_number);
        }else{
        fprintf_bits(fp,head->neg_bits,input_number);
        }
        head = head -> next;
    }
    fflush(fp);
    rewind(fp);
    fclose(fp);
}

void make_cube_list(const char* filename, Cube** cube_list, int *input_num){
    FILE *fp = fopen(filename,"r");
    if(fp==NULL){
        fprintf(stderr,"Cannot Open make_cube list input_file\n");
         fclose(fp);
        return;
    }
    char line[256];
    int input_number = 0;
    int is_first_line = 1;
    Cube *tail = NULL;
    Cube *new_cube = NULL;
    while(fgets(line,sizeof(line),fp) != NULL){
        if(is_first_line){
            input_number = strlen(line)-1;
            (*input_num) = input_number;
            is_first_line = 0;
        }
        new_cube = parse_cube_string(line,input_number);
        append_to_list(cube_list, &tail, new_cube);
    }
    fclose(fp);
}

//cubeのposとnegを合体させて1,0,-で表示させる.
void fprintf_cube_combined(FILE* stream, const Cube* c, int n, const char* output) {
    if (c == NULL) {
        fprintf(stream, "(NULL)\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        // 最上位ビット（左端）から順番に1ビットずつチェックしていくためのマスク
        uint64_t mask = (1ULL << (n - 1 - i));
        
        int has_pos = (c->pos_bits & mask) != 0;
        int has_neg = (c->neg_bits & mask) != 0;

        if (has_pos && !has_neg) {
            // pos=1, neg=0 のときは「1」
            fprintf(stream, "1");
        } else if (!has_pos && has_neg) {
            // pos=0, neg=1 のときは「0」
            fprintf(stream, "0");
        } else if (has_pos && has_neg) {
            // pos=1, neg=1 のときは「- (ドントケア)」
            fprintf(stream, "-");
        } else {
            // pos=0, neg=0 のときは「矛盾（消滅）」
            // 通常のキューブでは発生しませんが、デバッグ用に 'X' としています
            fprintf(stream, "X");
        }
    } 
    // キューブの終わりに改行を入れる
    if(output != NULL && output[0] != '\0'){
        fprintf(stream, " %s\n", output);
    } else { 
    fprintf(stream, "\n");
    }
}

/**
 * Cubeのリスト（集合）全体をまとめてファイルに出力する関数
 */
void fprintf_cube_list_combined(const char *stream, const Cube* head, int n, const char* output) {

    FILE *fp = fopen(stream,"w");
    if(fp == NULL){
        fprintf(stderr,"Cannot Open combined_cube_list file.\n");
        return;
    }
    const Cube* curr = head;
    if (curr == NULL) {
        fprintf(fp, "(Empty List)\n");
        fclose(fp);
        return;
    }
    while (curr != NULL) {
        fprintf_cube_combined(fp, curr, n, output);
        curr = curr->next;
    }

    fflush(fp);
    fclose(fp);
 
}
