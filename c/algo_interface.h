#ifndef ALGO_INTERFACE_H
#define ALGO_INTERFACE_H

#include <stdint.h>
#include <stddef.h>

/* Sorting */

typedef struct {
    int    *array;        /* current state of the array*/
    size_t  size;         /* number of elements*/
    int     compare_a;    /* index being compared (-1 if none)*/
    int     compare_b;    /* index being compared (-1 if none)*/
    int     swap_a;       /* index being swapped  (-1 if none)*/
    int     swap_b;       /* index being swapped  (-1 if none)*/
    uint64_t comparisons; /* running total of comparisons*/
    uint64_t swaps;       /* running total of swaps*/
    int     done;         /* 1 when sort is complete*/
} SortFrame;

/* called once per step with the current frame; user_data is caller context */
typedef void (*SortCallback)(const SortFrame *frame, void *user_data);

/* Pathfinding*/

typedef enum {
    CELL_UNVISITED = 0,
    CELL_OPEN,            /* in the frontier / open set*/
    CELL_CLOSED,          /* already evaluated*/
    CELL_PATH,            /* part of the final path*/
    CELL_WALL             /* impassable*/
} CellState;

typedef struct {
    CellState *grid;      /* flat row-major array [rows * cols]*/
    int        rows;
    int        cols;
    int        current;   /* flat index of node being evaluated */
    int        done;      /* 1 when path found or exhausted*/
    int        found;     /* 1 if a path exists, 0 if no path*/
    uint64_t   steps;     /* running total of nodes evaluated**/
} PathFrame;

typedef void (*PathCallback)(const PathFrame *frame, void *user_data);

#endif /* ALGO_INTERFACE_H */
