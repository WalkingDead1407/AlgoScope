CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -fPIC
SHARED = -shared

TARGETS = libbubble.so libmerge.so libastar.so libdijkstra.so

all: $(TARGETS)

libbubble.so: bubble_sort.c
	$(CC) $(CFLAGS) $(SHARED) -o $@ $<

libmerge.so: merge_sort.c
	$(CC) $(CFLAGS) $(SHARED) -o $@ $<

libastar.so: astar.c heap.c
	$(CC) $(CFLAGS) $(SHARED) -o $@ $^

libdijkstra.so: dijkstra.c heap.c
	$(CC) $(CFLAGS) $(SHARED) -o $@ $^

clean:
	rm -f *.so *.o

test_sort: test_sort.c bubble_sort.c merge_sort.c
	$(CC) $(CFLAGS) -o test_sort test_sort.c bubble_sort.c merge_sort.c
	./test_sort
