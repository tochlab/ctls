#include <stdio.h>

#include "lib/listdir.h"

void test_file( char *filename ) {
    printf("%s\n", filename);
    return;
}

int main(int argc, char**argv) {
    const char default_dir[] = ".";
    char *dirname = NULL;
    if(argc == 1) {
        dirname = (char *) &default_dir[0];
    }
    else {
        dirname = argv[1];
    }
    
    list_dir(dirname, test_file);
}
