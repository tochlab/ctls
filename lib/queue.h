#include <stddef.h>


typedef struct {
    void *next;
    void *payload;
} queue_element_t;

typedef struct {
    queue_element_t *head;
    queue_element_t *tail;
    int size;
    int lock;
} queue_t;


queue_t *queue_new( void );
void    queue_push(queue_t *q, void *elem);
void    *queue_pop(queue_t *q);
size_t  queue_size(queue_t *q);
