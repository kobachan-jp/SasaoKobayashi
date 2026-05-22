#include <stdio.h>
#include "filterfile.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr,"Input The File\n");
        return 0;
    }
    
    trim_newline(argv[1]);
    
    return 0;
}