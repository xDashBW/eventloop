#ifndef __EVENTLOOP_EVENT_H
#define __EVENTLOOP_EVENT_H

/**
   创建 eventloop
 */
struct eventloop *eventloop_new();

/**
   销毁 evnetloop
 */
void eventloop_destory(struct eventloop *loop);

/**
   事件分发
   @loop:eventloop
 */
void eventloop_dispatch(struct eventloop *loop);

/* 事件回调 */
typedef void (*callback)(int fd, short wake_event, void *user);

#define WATCHER_ONLY_READ 5
#define WATCHER_ONLY_WRITE 6

/**
   创建 watcher
   @loop: 要加入的 eventloop
   @fd: 改事件代表的文件描述符
   @event: 要监听的事件
   @callback: 该事件唤醒时要执行的回调
 */
struct watcher *watcher_new(int fd, short event, void *user, callback cb,
                            struct eventloop *loop);

/**
   把 watcher 注册到 eventloop 中
   @eventloop: 调度 loop
   @wather: 事件
 */
void watcher_add(struct watcher *watcher, struct eventloop *loop);

/**
   把 watcher 从 eventloop 中移除
   @eventloop: 调度 loop
   @watcher: 事件
 */
void watcher_del(struct watcher *watcher, struct eventloop *loop);

#endif
