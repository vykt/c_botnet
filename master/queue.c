#include <string.h>
#include <stdint.h>

#include "queue.h"

void queue_init(struct queue * q) {

	memset(q->content, 0, sizeof(q->content));
	q->start_index = 0;
	q->end_index = 0;

}

//0 - success, -1 - fail
int queue_push(struct queue * q, uint16_t val) {

	p_end = q->end_index;
	p_str = q_start_index;
	//if queue is full
	if (p_end == p_str) {
		return -1;
	}

	q->content[q->end_index] = val;
	
	if (p_end + 1 == QUEUE_SIZE) {
		q->end_index = 0;
	} else {
		q->end_index = q->end_index + 1;
	}

	return 0;
}

//n - success, -1 - fail
uint16_t queue_pop(struct queue * q) {

	uint16_t ret;

	if (q->content[start_index] == 0) {
		return -1;
	} else {

		ret = q->content[start_index];
		q->content[start_index] = 0;
		q->start_index = q->start_index + 1;
		if (q->start_index == QUEUE_SIZE) q->start_index = 0;
		return ret;
	}
}

//0 = empty, 1 = not empty
int queue_not_empty(struct queue * q) {
	if (q->content[start_index] == 0) {
		return 1;
	} else {
		return 0;
	}

}
