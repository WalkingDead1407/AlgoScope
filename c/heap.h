#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>

#define MAX_GRID_CELLS 4096    /* max rows*cols = 64x64 grid */

typedef struct {
    int   index;   /* flat grid index of the node */
    float cost;    /* f cost for A*, g cost for Dijkstra */
} HeapNode;

typedef struct {
    HeapNode data[MAX_GRID_CELLS];
    size_t   size;
} MinHeap;

void     heap_init(MinHeap *heap);
void     heap_push(MinHeap *heap, int index, float cost);
HeapNode heap_pop(MinHeap *heap);
int      heap_empty(const MinHeap *heap);

#endif /* HEAP_H */
