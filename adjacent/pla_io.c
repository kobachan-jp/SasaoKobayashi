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
        printf("This ch is : %c\n",ch);
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

void save_cube_list(const char* output_filename, Cube** cube_list, bool n){
    FILE *fp = fopen(output_filename,"w");
    if(fp==NULL){
        fprintf(stderr,"Cannot Open save_cube list output_file\n");
        return;
    }
    Cube *head = cube_list;
    while(head != NULL){
        if(n == true){
        fprintf(fp,"%016lx\n",head->pos_bits);
        }else{
        fprintf(fp,"%016lx\n",head->neg_bits);
        }
        head = head -> next;
    }
    fflush(fp);
    rewind(fp);
    fclose(fp);
}

void make_cube_list(const char* filename, Cube** cube_list){
    FILE *fp = fopen(filename,"r");
    if(fp==NULL){
        fprintf(stderr,"Cannot Open make_cube list input_file\n");
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
            is_first_line = 0;
            printf("This input number is %d\n", input_number);
            printf("-------------------------------------------\n");
        }
        new_cube = parse_cube_string(line,input_number);
        append_to_list(cube_list, &tail, new_cube);
    }
    fclose(fp);
}