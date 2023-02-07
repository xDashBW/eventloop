#ifndef __EVENTLOOP_RINGBUF_H
#define __EVENTLOOP_RINGBUF_H

#include <assert.h>
#include <stdlib.h>

struct ringbuf {
	unsigned char *data;
	size_t sz;
	size_t head;
	size_t tail;
};

static inline size_t __ringbuf_lsize(struct ringbuf *r)
{
	//    tail ... head
	//     ...head  tail...
}

static inline size_t __ringbuf_rsize(struct ringbuf *r)
{
}

static inline size_t ringbuf_push(struct ringbuf *r, void *data, size_t size)
{
	assert(data);
	const char *lptr = (const char *)data;
	size_t sz = size > r->sz ? r->sz : size;

	if (sz >)

		return sz;
}

#endif
