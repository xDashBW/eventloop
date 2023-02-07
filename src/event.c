#include "include/event.h"

#include <assert.h>
#include <stdlib.h>

#include "event-internal.h"
#include "list.h"

const struct pollops *pollops = &epollops;

struct eventloop *eventloop_new()
{
	struct eventloop *loop =
		(struct eventloop *)malloc(sizeof(struct eventloop));
	assert(loop);

	list_head_init(&loop->wake_head);

	loop->fd_heads = (struct list_head *)malloc(sizeof(struct list_head) *
						    FDS_SIZE_INIT);
	assert(loop->fd_heads);

	loop->fds_sz = FDS_SIZE_INIT;

	for (int i = 0; i < FDS_SIZE_INIT; ++i) {
		list_head_init(&loop->fd_heads[i]);
	}

	loop->pollops = pollops;
	loop->pollops_obj = loop->pollops->new(loop);
	assert(loop->pollops_obj);
}

void eventloop_destory(struct eventloop *loop)
{
	free(loop->fd_heads);
	loop->pollops->destory(loop->pollops_obj);
	free(loop);
}

void eventloop_dispatch(struct eventloop *loop)
{
    while (1) {
        loop->pollops->dispatch(loop->pollops_obj);

        struct watcher *w;
        list_foreach(w, &loop->wake_head, wake_entry) {
            w->callback(w->fd, w->wake_event, w->user);
        }
    }
}

struct watcher *watcher_new(int fd, short event, void *user, callback cb,
			    struct eventloop *loop)
{
	struct watcher *w = (struct watcher *)malloc(sizeof(struct watcher));
	assert(w != 0);

	list_head_init(&w->wake_entry);
	list_head_init(&w->fd_entry);
	w->fd = fd;
	w->wait_event = event;
	w->wake_event = 0;
	w->user = user;
	w->cb = cb;
	w->loop = loop;

	return w;
}

void watcher_add(struct watcher *watcher, struct eventloop *loop)
{
	int fd = watcher->fd;
	struct list_head *fd_head;

	if (fd >= loop->fds_sz) {
		loop->fd_heads = realloc(loop->fd_heads, 2 * loop->fds_sz);
		assert(loop->fd_heads);
		loop->fds_sz *= 2;
	}

	fd_head = &loop->fd_heads[fd];

    /* 该 fd 添加新 watcher */
	if (list_empty(&fd_head)) {
		loop->pollops->add(loop->pollops_obj, watcher->fd,
				   watcher->wait_event);
	} else {
        /* 该 fd 在已有的 watcher 上继续添加 watcher */
		loop->pollops->mod(loop->pollops_obj, watcher->fd,
				   WATCHER_READ_AND_WRITE);
	}

	list_add_tail(&watcher->wait_event, fd_head);
}

void watcher_del(struct watcher *watcher, struct eventloop *loop)
{
	int fd = watcher->fd;
	struct list_head *fd_head = &loop->fd_heads[fd];

	if (list_empty(head))
		assert(0);

	list_del_init(&watcher->fd_entry);

	if (list_empty(fd_head)) {
        /* Fd 最后一个 watcher, 删除后代表不监听该 fd */
		loop->pollops->del(loop->pollops_obj, watcher->fd);
	} else {
        /* 修改 */
		struct list_head *next = fd_head->next;
		struct watcher *e = list_entry(next, struct watcher, fd_entry);
		loop->pollops->mod(loop->pollops_obj, e->fd, e->wait_event);
	}
}
