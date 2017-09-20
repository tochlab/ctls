#include "cbuf.h"
#include <string.h>

void cbuf_init(cbuf_t *buf, void *ptr, uint32_t maxsize8)
{
    buf->data = ptr;
    buf->size8 = maxsize8;
    buf->readpos8 = 0;
    buf->writepos8 = 0;
}

void cbuf_write(cbuf_t *buf, void *src, uint32_t len8)
{
    char *write_ptr = ((char *) buf->data) + buf->writepos8;

    if( buf->writepos8 + len8 <= buf->size8) {
        /* char *write_ptr = ((char *) buf->data) + buf->writepos8; */
        memcpy(write_ptr, src, len8);
        buf->writepos8 += len8;
    }
    else {
        uint32_t taillen8 = buf->size8 - buf->writepos8;
        char *src_ptr = 0; /*  warning: ISO C90 forbids mixed declarations and code */
        /* char *write_ptr = ((char *) buf->data) + buf->writepos8; */

        memcpy(write_ptr, src, taillen8);
        buf->writepos8 = 0;
        write_ptr = ((char *) buf->data) + buf->writepos8;
        src_ptr = ((char *) src) + taillen8;
        memcpy(write_ptr, src_ptr, len8 - taillen8);
        buf->writepos8 = len8 - taillen8;
    }
}

void cbuf_read(cbuf_t *buf, void *dst, uint32_t len8)
{
    char *readptr = ((char *) buf->data) + buf->readpos8;
    
    if( buf->readpos8 + len8 <= buf->size8) {
/*        char *readptr = ((char *) buf->data) + buf->readpos8; */
        memcpy(dst, readptr, len8);
        buf->readpos8 += len8;
    }
    else {
        uint32_t headlen8 = buf->size8 - buf->readpos8;
        char *dst_ptr = 0;  /*  warning: ISO C90 forbids mixed declarations and code */
        memcpy(readptr, dst, headlen8);
        buf->readpos8 = 0;
        readptr = (char *) buf->data;
        dst_ptr = ((char *) dst) + headlen8;
        memcpy(dst_ptr, readptr, len8-headlen8);
    }
}
