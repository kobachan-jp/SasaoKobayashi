#include <stdio.h>
#include "file.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Input The File\n");
    }
    
    trim_newline(argv[1]);
    
    return 0;
}