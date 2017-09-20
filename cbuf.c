#include "lib/cbuf.h"
#include <stdio.h>
#include <string.h>

#define TEST_BUF_LEN8 ( 1526 )
#define WRITE_BUF_LEN8 ( 120 )
#define READ_BUF_LEN8 ( 37 )

int main()
{
    uint8_t test_buf[TEST_BUF_LEN8] = { 0 };

    uint8_t writebuf[WRITE_BUF_LEN8] = { 0 };
    uint8_t readbuf[READ_BUF_LEN8] = { 0 };

    for(uint8_t i = 0;i<WRITE_BUF_LEN8;i++) {
        writebuf[i] = i;
    }

    cbuf_t cbuf;

    cbuf_init(&cbuf, &test_buf[0], TEST_BUF_LEN8);
    for(int i = 0;i<528;i++) {
        cbuf_write(&cbuf, writebuf, WRITE_BUF_LEN8);
    }
    for( int i = 0;i<TEST_BUF_LEN8;i++) {
        printf("%d ", test_buf[i]);
    }

    printf("\n\n\n");

    for(int i = 0;i<487;i++) {
        cbuf_read(&cbuf, readbuf, READ_BUF_LEN8);
        for( int x = 0;x<READ_BUF_LEN8;x++) {
            printf("%d ", readbuf[x]);
        }
        printf("\n");
    }
}
