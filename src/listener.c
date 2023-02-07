
#include "listener.h"

#include "event-internal.h"

struct tcp_listener {
	struct watcher listen_watcher;
};
