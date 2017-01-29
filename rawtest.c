#include "lib/rawfs.h"

int main() {

    char *devs[]  = {
        "device_1.bin",
        "device_2.bin"
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

    rawfs_write(fs, 1, testbuf, TESTBUFLEN * sizeof(int));
    rawfs_write(fs, 2, testbuf, TESTBUFLEN * sizeof(int));
    rawfs_write(fs, 3, testbuf, TESTBUFLEN * sizeof(int));

    rawfs_dump_maps(fs);

}