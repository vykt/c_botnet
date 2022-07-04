#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#define QUEUE_SIZE 256

/*
 *	My attempt at having a queue class/object in C.
 */

struct queue {

	uint16_t content[QUEUE_SIZE];
	int end_index;
	int start_index;

};

void queue_init(struct queue * q);
int queue_push(struct queue * q, uint16_t val);
uint16_t queue_pop(struct queue * q);
int queue_not_empty(struct queue * q);

#endif
