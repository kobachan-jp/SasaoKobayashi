#include <stdio.h>
#include <string.h>
#include "pla_io.h"
#include "cube_restrict.h"

int main(int argc, char *argv[]){
    init_cube_pool();
Cube *uni = create_universe_cube();
FILE *fp = fopen("restrict.txt","w");
if(fp == NULL){
    fprintf(stderr,"Cannot Open restrict uni\n");
    return 0;
}
fprintf(fp,"uni cube -> pos_bits:\n");
fprintf_bits(fp,uni->pos_bits,4);
fprintf(fp,"uni cube -> neg_bits:\n");
fprintf_bits(fp,uni->neg_bits,4);

fflush(fp);
fclose(fp);
free_cube(uni);
return 0;
}