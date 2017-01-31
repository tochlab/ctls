#include <stdlib.h>
#include "lib/rawfs.h"

int main() {

    char *devs[]  = {
        "device1.bin",
        "device2.bin"
    };

    #define TESTCOUNT 128
    #define TESTBUFLEN 6 * 1024*1024

    rawfs_t *fs = rawfs_new(devs, sizeof(devs)/sizeof(char*), TESTCOUNT);

    if( fs == NULL ) {
        return 1;
    }

    int *testbuf = calloc(sizeof(int), TESTBUFLEN);

    fprintf(stderr,"Writing ...");
    for(int test_idx = 0;test_idx<TESTCOUNT;test_idx++) {
        for( int i = 0;i< TESTBUFLEN;i++) {
            testbuf[ i ] = i + test_idx;
        }
        
        int result = rawfs_write(fs, testbuf, TESTBUFLEN * sizeof(int));
        if( result != test_idx ) {
            fprintf(stderr,"test_idx warning %d/%d\n", result, test_idx);
        }
    }
    fprintf(stderr," OK\n");

    int *readbuf = calloc(sizeof(int), TESTBUFLEN);

    fprintf(stderr,"Reading ...");
    for(int test_idx = 0;test_idx<TESTCOUNT*100;test_idx++) {
        int rand_idx = rand() % 128;

        for( int i = 0;i< TESTBUFLEN;i++) {
            testbuf[ i ] = i + rand_idx;
        }        

        rawfs_read(fs, rand_idx, readbuf, TESTBUFLEN * sizeof(int));
        for( int i = 0;i<TESTBUFLEN;i++) {
            if(readbuf[i] != testbuf[i]) {
                fprintf(stderr,"WRONG READ!\n");
                return -1;
            }
        }
    }
    fprintf(stderr," OK\n");

    free(testbuf);
    free(readbuf);

    // rawfs_dump_maps(fs);

}
