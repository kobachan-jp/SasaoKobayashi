#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include"cube_pool.h"

uint64_t binary_to_uint64(char *s){
    uint64_t x = 0;

    for(int i = 0; s[i] != '\0'; i++){
        x <<= 1;
        if(s[i] == '1'){
            x |= 1;
        }
    }
    return x;
}

/*
入力ファイルの行末が\r\nとならないように\0とする.
Windowsのファイルをubuntuで実行できる.
ubuntuのファイルはwindowsでは特に何もしなくても実行できる.
*/
void trim_newline(const char* filename){

    char line[256];

    FILE* fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Cannot Open the file.\n");
        return;
    }else{
        printf("Opened the file.\n");
    }
    while(fgets(line, sizeof(line),fp) != NULL){

        line[strcspn(line, "\r\n")] = '\0';
        printf("%s\n",line);
    }

    fclose(fp);
    return;


}