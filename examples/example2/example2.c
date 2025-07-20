#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define BUF_SIZE 50

typedef struct {
	char buffer[BUF_SIZE];
	int head;
	int tail;
	int count;
} RingBuffer;

void rb_init(RingBuffer *rb) {
	rb->head = 0;
	rb->tail = 0;
	rb->count = 0;
}

bool is_empty(RingBuffer *rb) {
	return rb->count == 0;
}

bool is_full(RingBuffer *rb) {
	return rb->count == BUF_SIZE;
}

bool enqueue(RingBuffer *rb, char data) {
	if (is_full(rb)) return false;
	rb->buffer[rb->head] = data;
	rb->head = (rb->head + 1) % BUF_SIZE;
	rb->count++;
	return true;
}

bool dequeue(RingBuffer *rb, char *out) {
	if (is_empty(rb)) return false;
	*out = rb->buffer[rb->tail];
	rb->tail = (rb->tail + 1) % BUF_SIZE;
	rb->count--;
	return true;
}

int main() {

	RingBuffer rb;
	rb_init(&rb);

	for (int i = 0; i < BUF_SIZE+5; i++) {
		if (enqueue(&rb, i))
			printf("Enqueued %d\n", i);
		else
			printf("Buffer full, couldn't enqueue %d\n", i);
	}

	char val;
	while (dequeue(&rb, &val))
		printf("Dequeued %d\n", val);

	return 0;
}

