#include <stdlib.h>
#include <sys/time.h>
#include "lib/rawfs.h"

int main() {

    char *devs[]  = {
        "/tmp/device1.bin",
        "/tmp/device2.bin"
    };

    #define TESTCOUNT 100
    #define TESTBUFLEN 6*1024*1024

    rawfs_t *fs = rawfs_new(devs, sizeof(devs)/sizeof(char*), TESTCOUNT);

    if( fs == NULL ) {
        return 1;
    }

    int *testbuf = calloc(sizeof(int), TESTBUFLEN);
    struct timeval start;
    struct timeval end;
    
    for(int test_idx = 0;test_idx<TESTCOUNT;test_idx++) {
        for( int i = 0;i< TESTBUFLEN;i++) {
            testbuf[ i ] = i + test_idx;
        }
        usleep(500000);
        //fprintf(stderr,"Writing ... %d ", test_idx);
        gettimeofday(&start, NULL);
        int result = rawfs_write(fs, testbuf, TESTBUFLEN * sizeof(int));
        gettimeofday(&end, NULL);
        if( result != test_idx ) {
            fprintf(stderr,"test_idx warning %d/%d\n", result, test_idx);
        }
    }
    fprintf(stderr," OK\n");

    int *readbuf = calloc(sizeof(int), TESTBUFLEN);

    for(int test_idx = 0;test_idx<TESTCOUNT*100;test_idx++) {
        int rand_idx = rand() % TESTCOUNT;
        fprintf(stderr,"Reading ... %d\r", rand_idx);

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
