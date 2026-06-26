#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

void heap_init(MinHeap *heap) {heap->size = 0;}

int heap_empty(const MinHeap *heap) {return heap->size == 0;}

/*internal helpers*/

static void swap(HeapNode *a, HeapNode *b)
{
    HeapNode tmp = *a;
    *a = *b;
    *b = tmp;}

static void bubble_up(MinHeap *heap, size_t i)
{    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (heap->data[parent].cost <= heap->data[i].cost)
            break;
        swap(&heap->data[parent], &heap->data[i]);
        i = parent;}
}

static void bubble_down(MinHeap *heap, size_t i)
{    while (1) {
        size_t left     = 2 * i + 1;
        size_t right    = 2 * i + 2;
        size_t smallest = i;

        if (left  < heap->size && heap->data[left].cost  < heap->data[smallest].cost)
            smallest = left;
        if (right < heap->size && heap->data[right].cost < heap->data[smallest].cost)
            smallest = right;

        if (smallest == i)
            break;

        swap(&heap->data[i], &heap->data[smallest]);
        i = smallest;}
}

/*public api*/

void heap_push(MinHeap *heap, int index, float cost)
{   if (heap->size >= MAX_GRID_CELLS) {
        fprintf(stderr, "heap overflow: exceeded MAX_GRID_CELLS (%d)\n", MAX_GRID_CELLS);
        exit(1);
    }

    heap->data[heap->size].index = index;
    heap->data[heap->size].cost  = cost;
    bubble_up(heap, heap->size);
    heap->size++;
}

HeapNode heap_pop(MinHeap *heap)
{   if (heap->size == 0) {
        fprintf(stderr, "heap underflow: pop on empty heap\n");
        exit(1);
    }

    HeapNode top  = heap->data[0];
    heap->size--;
    heap->data[0] = heap->data[heap->size];
    bubble_down(heap, 0);

    return top;
}
