#include "algo_interface.h"
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

#define INF 1e9f

/*manhattan distance heuristic */

static float heuristic(int index, int goal, int cols)
{   int curr_row = index / cols;
    int curr_col = index % cols;
    int goal_row = goal  / cols;
    int goal_col = goal  % cols;
    return (float)(abs(curr_row - goal_row) + abs(curr_col - goal_col));
}

/*neighbour helper*/

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
{   int path[MAX_GRID_CELLS];
    int path_len = 0;
    int current  = goal;
    while (current != start) {
        path[path_len++] = current;
        current = came_from[current];
    }
    path[path_len++] = start;
    for (int i = path_len - 1; i >= 0; i--) {
        grid[path[i]]  = CELL_PATH;
        frame->current = path[i];
        frame->steps++;
        cb(frame, user_data);
    }
}

void a_star(CellState *grid, int rows, int cols,
            int start, int goal,
            PathCallback cb, void *user_data)
{   int total = rows * cols;
    /* g[i] — actual cost from start to i */
    float g[MAX_GRID_CELLS];
    for (int i = 0; i < total; i++)
        g[i] = INF;
    g[start] = 0.0f;

    /* f[i] = g[i] + h(i) — estimated total cost through i */
    float f[MAX_GRID_CELLS];
    for (int i = 0; i < total; i++)
        f[i] = INF;
    f[start] = heuristic(start, goal, cols);

    /* came_from[i] — parent node on cheapest path to i */
    int came_from[MAX_GRID_CELLS];
    for (int i = 0; i < total; i++)
        came_from[i] = -1;

    /* closed set — node has been finalized */
    int closed[MAX_GRID_CELLS];
    for (int i = 0; i < total; i++)
        closed[i] = 0;

    MinHeap heap;
    heap_init(&heap);
    heap_push(&heap, start, f[start]);
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
        /* skip stale heap entries */
        if (closed[current])
            continue;

        closed[current] = 1;
        grid[current]   = CELL_CLOSED;
        frame.current   = current;
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
                f[nb]         = new_g + heuristic(nb, goal, cols);
                came_from[nb] = current;
                grid[nb]      = CELL_OPEN;
                frame.current = nb;
                frame.steps++;
                cb(&frame, user_data);
                heap_push(&heap, nb, f[nb]);
            }
        }
    }

    /* emit done */
    frame.done = 1;
    cb(&frame, user_data);
}


/*heuristic()-- manhattan distance between current node and goal. Computed once per neighbour evaluation. Since all edge costs are 1 and the heuristic never overestimates, A* is guaranteed optimal

f[] array-- dijkstra orders the heap by g alone. A* adds an f[] array and orders by f = g + h. This is the entire difference algorithmically - one extra array and one extra addition per neighbour\

f[start]-- heuristic(start, goal, cols) - start node's g is 0 so its f is purely the heuristic. Every other node starts at INF for both g and f
*/
