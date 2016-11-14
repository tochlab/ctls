#include "queue.h"

#include <stdlib.h>

queue_t *queue_new( void ) {
	queue_t *result = (queue_t *) malloc(sizeof(queue_t));
        result->head = NULL;
        result->tail = NULL;
        result->size = 0;
	return result;
}

void queue_push(queue_t *q, void *e) {
    queue_element_t *newelement = (queue_element_t *) malloc(sizeof(queue_element_t));

    newelement->payload = e;

    if(q->size == 0) {
	q->head = newelement;
    }
    else {
        q->tail->next = newelement;
    }
    q->size++;
    q->tail = newelement;
}

void *queue_pop(queue_t *q) {
    if(q->size == 0) {
	return NULL;
    }
    void *result = q->head->payload;
    queue_element_t *head_element = q->head;
    q->head = q->head->next;
    free(head_element);
    q->size--;
    return result;
}

void *queue_front(queue_t *q) {
    void *result = q->head;
    return result;
}

void *queue_back(queue_t *q) {
    void *result = q->tail;
    return result;
}

size_t queue_size(queue_t *q) {
    size_t result = q->size;
    return result;
}
