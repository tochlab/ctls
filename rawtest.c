#include "lib/rawfs.h"

int main() {

    char *devs[]  = {
        "device1.bin",
        "device2.bin"
    };

    #define TESTBUFLEN 1024
    int testbuf[TESTBUFLEN];
    for( int i = 0;i< TESTBUFLEN;i++) {
        testbuf[ i ] = i;
    }

    rawfs_t *fs = rawfs_new(devs, sizeof(devs)/sizeof(char*));

    if( fs == NULL ) {
        return 1;
    }

    for(int test_idx = 0;test_idx<1000;test_idx++) {
        rawfs_write(fs, test_idx, testbuf, TESTBUFLEN * sizeof(int));
    }
    rawfs_dump_maps(fs);

}
