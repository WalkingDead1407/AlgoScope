#include "algo_interface.h"
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

#define INF 1e9f

/*neighbour help*/

static int get_neighbours(int index, int rows, int cols, int neighbours[4])
{   int count = 0;
    int row   = index / cols;
    int col   = index % cols;
    if (row > 0)        neighbours[count++] = index - cols; /* up*/
    if (row < rows - 1) neighbours[count++] = index + cols; /* down*/
    if (col > 0)        neighbours[count++] = index - 1;    /* left*/
    if (col < cols - 1) neighbours[count++] = index + 1;    /* right*/
    return count;
}

/*path reconstruction*/

static void reconstruct_path(int *came_from, int start, int goal,
                              CellState *grid,
                              PathFrame *frame, PathCallback cb, void *user_data)
{ /* trace back from goal to start */
    int path[MAX_GRID_CELLS];
    int path_len = 0;
    int current  = goal;
    while (current != start) {
        path[path_len++] = current;
        current = came_from[current];
    }
    path[path_len++] = start;
    /* mark cells as CELL_PATH and emit a frame per cell */
    for (int i = path_len - 1; i >= 0; i--) {
        grid[path[i]]   = CELL_PATH;
        frame->current  = path[i];
        frame->steps++;
        cb(frame, user_data);
    }
}

/*Dijkstra*/

void dijkstra(CellState *grid, int rows, int cols,
              int start, int goal,
              PathCallback cb, void *user_data)
{   int total = rows * cols;
    /* cost_array — g[i] is the shortest known distance from start to i */
    float g[MAX_GRID_CELLS];
    for (int i = 0; i < total; i++)
        g[i] = INF;
    g[start] = 0.0f;
    /* came_from[i] = which node we came from to reach i */
    int came_from[MAX_GRID_CELLS];
    for (int i = 0; i < total; i++)
        came_from[i] = -1;
    /* closed set — node has been finalized */
    int closed[MAX_GRID_CELLS];
    for (int i = 0; i < total; i++)
        closed[i] = 0;

    MinHeap heap;
    heap_init(&heap);
    heap_push(&heap, start, 0.0f);
    PathFrame frame;
    frame.grid  = grid;
    frame.rows  = rows;
    frame.cols  = cols;
    frame.steps = 0;
    frame.done  = 0;
    frame.found = 0;

    while (!heap_empty(&heap)) {
        HeapNode node = heap_pop(&heap);
        int current   = node.index;
        /* skip if already visited */
        if (closed[current])
            continue;

        closed[current]  = 1;
        grid[current]    = CELL_CLOSED;
        frame.current    = current;
        frame.steps++;
        cb(&frame, user_data);

        /* goal reached */
        if (current == goal) {
            frame.found = 1;
            reconstruct_path(came_from, start, goal, grid, &frame, cb, user_data);
            break;
        }
        /* expand neighbours */
        int neighbours[4];
        int count = get_neighbours(current, rows, cols, neighbours);

        for (int i = 0; i < count; i++) {
            int nb = neighbours[i];

            if (closed[nb] || grid[nb] == CELL_WALL)
                continue;

            float new_g = g[current] + 1.0f;

            if (new_g < g[nb]) {
                g[nb]         = new_g;
                came_from[nb] = current;
                grid[nb]      = CELL_OPEN;
                frame.current = nb;
                frame.steps++;
                cb(&frame, user_data);
                heap_push(&heap, nb, new_g);
            }
        }
    }

    /* emit done */
    frame.done = 1;
    cb(&frame, user_data);
}

/* i[i]=INF-- every node starts at infinite cost. Only the start is 0.0f. A node gets updated only when we find a cheaper path to it

closed[] arr-- when a node is popped from the heap it may have been finalized via a cheaper path which was found earlier. The if closed[current] continue guard handles duplicate heap entries cheaply without needing to update/remove existing heap entries

came_from[]-- every time we find a cheaper path to a neighbour we record where we came from. This is what reconstruct_path traces back through. */
