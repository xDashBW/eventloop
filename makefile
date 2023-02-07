# libeventloop.so: src/event.o src/epoll.o

# src/event.o: src/event.c src/event-internel.h include/event.h src/list.h src/container_of.h


libeventloop.so: event.o epoll.o
	gcc event.o epoll.o -fPIC -shared -o libeventloop.so

event.o: src/event.c src/event-internal.h include/event.h src/list.h src/container_of.h
	gcc -I include src/event.c -c -o event.o

epoll.o: src/epoll.c src/event-internal.h include/event.h src/list.h src/container_of.h
	gcc -I include src/epoll.c -c -o epoll.o

clean:
	rm epoll.o
	rm event.o
	rm libeventloop.so

install:
	cp libeventloop.so /usr/lib/
	cp include/event.h /usr/include/eventloop/
