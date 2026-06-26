#include "algo_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algo_interface.h"

/*grid printing*/

static void print_grid(const CellState *grid, int rows, int cols,
                        int start, int goal)
{
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int idx = r * cols + c;

            if (idx == start)       printf("S ");
            else if (idx == goal)   printf("G ");
            else switch (grid[idx]) {
                case CELL_UNVISITED: printf(". "); break;
                case CELL_OPEN:      printf("o "); break;
                case CELL_CLOSED:    printf("x "); break;
                case CELL_PATH:      printf("* "); break;
                case CELL_WALL:      printf("# "); break;
                default:             printf("? "); break;
            }
        }
        printf("\n");
    }
}

/*callback */

static void path_callback(const PathFrame *frame, void *user_data)
{   const int *meta  = (const int *)user_data;  /* [start, goal, rows, cols] */
    int start        = meta[0];
    int goal         = meta[1];
    int rows         = meta[2];
    int cols         = meta[3];

    if (frame->done) {
        printf("DONE — steps: %lu | path %s\n\n",
               frame->steps, frame->found ? "FOUND" : "NOT FOUND");
        print_grid(frame->grid, rows, cols, start, goal);
        printf("\n");
        return;
    }

    /* only print grid on CELL_PATH frames so output isn't overwhelming */
    if (frame->grid[frame->current] == CELL_PATH) {
        printf("PATH cell %d (row %d, col %d)\n",
               frame->current,
               frame->current / cols,
               frame->current % cols);
    }
}

/*test runner */

static void run_test(const char *name,
                     void (*algo)(CellState *, int, int, int, int, PathCallback, void *),
                     CellState *grid, int rows, int cols,
                     int start, int goal)
{   printf("=== %s ===\n", name);
    printf("Grid: %dx%d | Start: %d | Goal: %d\n\n", rows, cols, start, goal);
    int meta[4] = {start, goal, rows, cols};
    algo(grid, rows, cols, start, goal, path_callback, meta);
}

int main(void)
{   /*test 1: open grid, clear path*/
    /*
      S . . . .
      . . . . .
      . . . . .
      . . . . G
     */
    {   int rows = 4, cols = 5;
        CellState grid[20];
        memset(grid, CELL_UNVISITED, sizeof(grid));
        int start = 0;
        int goal  = rows * cols - 1;   /* bottom-right */
        CellState g1[20], g2[20];
        memcpy(g1, grid, sizeof(grid));
        memcpy(g2, grid, sizeof(grid));
        printf(" Test 1: open grid \n\n");
        run_test("Dijkstra", dijkstra, g1, rows, cols, start, goal);
        run_test("A*", a_star,   g2, rows, cols, start, goal);
    }

    /* test 2: grid with walls forcing a detour*/
    /*
      S . . . .
      # # # # .
      . . . . .
      . . . . G
     */
    {   int rows = 4, cols = 5;
        CellState grid[20];
        memset(grid, CELL_UNVISITED, sizeof(grid));
        /* wall across row 1 except last cell */
        for (int c = 0; c < cols - 1; c++)
            grid[1 * cols + c] = CELL_WALL;
        int start = 0;
        int goal  = rows * cols - 1;
        CellState g1[20], g2[20];
        memcpy(g1, grid, sizeof(grid));
        memcpy(g2, grid, sizeof(grid));
        printf("Test 2: walled grid\n\n");
        run_test("Dijkstra", dijkstra, g1, rows, cols, start, goal);
        run_test("A*", a_star,   g2, rows, cols, start, goal);
    }

    /* test 3: no path possible*/
    /*
      S . . . .
      # # # # #
      . . . . .
      . . . . G
     */
    {   int rows = 4, cols = 5;
        CellState grid[20];
        memset(grid, CELL_UNVISITED, sizeof(grid));
        /* full wall across row 1 */
        for (int c = 0; c < cols; c++)
            grid[1 * cols + c] = CELL_WALL;
        int start = 0;
        int goal  = rows * cols - 1;
        CellState g1[20], g2[20];
        memcpy(g1, grid, sizeof(grid));
        memcpy(g2, grid, sizeof(grid));
        printf(" Test 3: no path\n\n");
        run_test("Dijkstra", dijkstra, g1, rows, cols, start, goal);
        run_test("A*", a_star,g2, rows, cols, start, goal);
    }

    return 0;
}
