#include <sys/epoll.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "event-internal.h"
#include "event.h"

#define EPOLL_EVENTS_INIT 1024

struct epoll {
	int epollfd;
	struct epoll_event *epevs;
	size_t epevs_sz;
	struct eventloop *loop;
};

#define type_to_epoll(name, voidname) \
	struct epoll *name = (struct epoll *)voidname

static void *epoll_new(struct eventloop *loop);
static void epoll_delete(void *ops);
static void epoll_add(void *ops, int fd, short events);
static void epoll_mod(void *ops, int fd, short events);
static void epoll_del(void *ops, int fd);
static void epoll_dispatch(void *ops);

const struct pollops epollops = { epoll_new, epoll_delete, epoll_add,
				  epoll_mod, epoll_del,	   epoll_dispatch };

static void *epoll_new(struct eventloop *loop)
{
	struct epoll *epoll = (struct epoll *)malloc(sizeof(struct epoll));
	assert(epoll != 0);

	epoll->epollfd = epoll_create(1024);
	assert(epoll->epollfd >= 0);

	epoll->epevs = (struct epoll_event *)malloc(sizeof(struct epoll_event) *
						    EPOLL_EVENTS_INIT);
	assert(epoll->epevs != 0);

	epoll->epevs_sz = EPOLL_EVENTS_INIT;

	epoll->loop = loop;

	return epoll;
}

static void epoll_delete(void *ops)
{
	type_to_epoll(epoll, ops);
	close(epoll->epollfd);
	free(epoll->epevs);
	free(epoll);
}

static void epoll_add(void *ops, int fd, short events)
{
	assert(events != WATCHER_READ_AND_WRITE);

	type_to_epoll(epoll, ops);
	int epev_events = 0;

	if (events == WATCHER_ONLY_READ) {
		epev_events |= EPOLLIN;
	} else {
		epev_events |= EPOLLOUT;
	}

	struct epoll_event epev;
	epev.events = epev_events;
	epev.data.fd = fd;
	epoll_ctl(epoll->epollfd, EPOLL_CTL_ADD, fd, &epev);
}

static void epoll_mod(void *ops, int fd, short events)
{
	type_to_epoll(epoll, ops);
	int epev_events = 0;

	if (events == WATCHER_READ_AND_WRITE) {
		epev_events |= EPOLLIN;
	} else if (events == WATCHER_ONLY_READ) {
		epev_events |= EPOLLIN;
	} else {
		epev_events |= EPOLLOUT;
	}

	struct epoll_event epev;
	epev.events = epev_events;
	epev.data.fd = fd;
	epoll_ctl(epoll->epollfd, EPOLL_CTL_MOD, fd, &epev);
}

static void epoll_del(void *ops, int fd)
{
	type_to_epoll(epoll, ops);
	epoll_ctl(epoll->epollfd, EPOLL_CTL_DEL, fd, NULL);
}

static void epoll_dispatch(void *ops)
{
	type_to_epoll(epoll, ops);

	int n = epoll_wait(epoll->epollfd, epoll->epevs, epoll->epevs_sz,
			   100000);

	for (int i = 0; i < n; ++i) {
		struct epoll_event *epev = &(epoll->epevs[i]);
		int fd = epev->data.fd;
		uint32_t revent = epev->events;
		struct list_head *fd_head = &epoll->loop->fd_heads[fd];
		struct watcher *w;
		short wakeup_event = 0;

		if (revent & (EPOLLHUP | EPOLLERR)) {
			wakeup_event = WATCHER_READ_AND_WRITE;
		} else {
			if (revent & (EPOLLIN | EPOLLOUT)) {
				wakeup_event = WATCHER_READ_AND_WRITE;
			} else if (revent & EPOLLIN) {
				wakeup_event = WATCHER_ONLY_READ;
			} else if (revent & EPOLLOUT) {
				wakeup_event = WATCHER_ONLY_WRITE;
			}
		}

		list_foreach_entry(w, fd_head, fd_entry)
		{
			if (WATCHER_READ_AND_WRITE == wakeup_event) {
				w->wake_event = w->wait_event;
				list_add_tail(&w->wake_entry,
					      &epoll->loop->wake_head);
			} else if (wakeup_event == w->wait_event) {
				w->wake_event = w->wait_event;
				list_add_tail(&w->wake_entry,
					      &epoll->loop->wake_head);
			}
		}
	}

	if (n == epoll->epevs_sz) {
		epoll->epevs = realloc(epoll->epevs,
				       sizeof(struct epoll_event) * 2 * n);
		assert(epoll->epevs);

		epoll->epevs_sz *= 2;
	}
}
