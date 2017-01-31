#include "lib/rawfs.h"

int main() {

    char *devs[]  = {
        "device1.bin",
        "device2.bin"
    };

    #define TESTCOUNT 128
    #define TESTBUFLEN 1024
    int testbuf[TESTBUFLEN];
    for( int i = 0;i< TESTBUFLEN;i++) {
        testbuf[ i ] = i;
    }

    rawfs_t *fs = rawfs_new(devs, sizeof(devs)/sizeof(char*), 1024);

    if( fs == NULL ) {
        return 1;
    }

    for(int test_idx = 0;test_idx<TESTCOUNT;test_idx++) {
        fprintf(stderr,"write %d ...", test_idx);
        int result = rawfs_write(fs, testbuf, TESTBUFLEN * sizeof(int));
        fprintf(stderr," %d\n", result);
    }

    int readbuf[TESTBUFLEN];

    for(int test_idx = 0;test_idx<TESTCOUNT;test_idx++) {
        fprintf(stderr,"read %d ...", test_idx);
        rawfs_read(fs, test_idx, readbuf, TESTBUFLEN * sizeof(int));
        for( int i = 0;i<TESTBUFLEN;i++) {
            if(readbuf[i] != testbuf[i]) {
                fprintf(stderr,"WRONG READ!\n");
                return -1;
            }
        }
        fprintf(stderr," OK\n");
    }


    // rawfs_dump_maps(fs);

}
