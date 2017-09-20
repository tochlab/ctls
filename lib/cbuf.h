#ifndef _CBUF_H_
#define _CBUF_H_

#include <linux/types.h>
#include <stdint.h>

typedef struct _cbuf_
{
    void *data;
    uint32_t writepos8;
    uint32_t readpos8;
    uint32_t size8;
} cbuf_t;

void cbuf_init(cbuf_t *buf, void *ptr, uint32_t size8);
void cbuf_write(cbuf_t *buf, void *data, uint32_t len8);
void cbuf_read(cbuf_t *buf, void *data, uint32_t len8);

#endif /* _CBUF_H_ */
