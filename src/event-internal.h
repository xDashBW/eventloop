#ifndef __EVENTLOOP_EVENT_INTERNAL_H
#define __EVENTLOOP_EVENT_INTERNAL_H

#include "list.h"

#define WATCHER_READ_AND_WRITE 7

struct eventloop;
struct pollops {
	void *(*new)(struct eventloop *);
	void (*destory)(struct eventloop *);
	void (*add)(void *, int, short);
	void (*mod)(void *, int, short);
	void (*del)(void *, int);
    void (*dispatch)(void *);
};

/*
  原来的名字为 event, 但是 event 用的太广泛了,
  为了把变量名更好区分, 所以改为 watcher, 意为观察某个事件之意.
 */
typedef void (*watcher_callback)(int, short, void *);
struct watcher {
	struct list_head wake_entry;
	struct list_head fd_entry;
	int fd;
	short wait_event;
	short wake_event;
	void *user;
	watcher_callback cb;
	struct eventloop *loop;
};

/*
  事件循环的核心结构, 调度队列
 */
struct eventloop {
	struct list_head wake_head;
	struct list_head *fd_heads;
#define FDS_SZ_INIT 1024
	size_t fds_sz;
	const struct pollops *pollops;
	void *pollops_obj;
};

#endif
