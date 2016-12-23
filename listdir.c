#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "lib/listdir.h"

static int testedfiles = 0;
static int sumsum = 0;

void test_file_open(char *filename) {
    char buf[2048];
    int sum = 0;
    //struct stat filestat;
    int f = open(filename, O_RDONLY);
    //fstat(fileno(f), &filestat);

    lseek(f, testedfiles, SEEK_SET);
    size_t readed = read(f, buf, 2048);
    for(size_t i = 0;i<readed;i++) {
        sum+=buf[i];
    }
    close(f);
    testedfiles++;
    sumsum += sum;
    return;
}

void test_file_file(char *filename) {
    char buf[2048];
    int sum = 0;
    //struct stat filestat;
    FILE *f = fopen(filename, "r");
    //fstat(fileno(f), &filestat);

    fseek(f, testedfiles, SEEK_SET);
    size_t readed = fread(buf, 1, 2048, f);
    for(size_t i = 0;i<readed;i++) {
        sum+=buf[i];
    }
    fclose(f);
    testedfiles++;
    sumsum += sum;
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
    
    list_dir(dirname, test_file_open);

    printf("Tested files %d\n", testedfiles);
    printf("SumSum %d\n", sumsum);
}
