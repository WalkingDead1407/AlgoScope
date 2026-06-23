#include "algo_interface.h"
#include <stddef.h>

void bubble_sort(int *array, size_t size, SortCallback cb, void *user_data)
{
    SortFrame frame;
    frame.array       = array;
    frame.size        = size;
    frame.comparisons = 0;
    frame.swaps       = 0;
    frame.done        = 0;

    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {

            /* emit compare step */
            frame.compare_a = (int)j;
            frame.compare_b = (int)j + 1;
            frame.swap_a    = -1;
            frame.swap_b    = -1;
            frame.comparisons++;
            cb(&frame, user_data);

            if (array[j] > array[j + 1]) {
                /* swap */
                int tmp      = array[j];
                array[j]     = array[j + 1];
                array[j + 1] = tmp;

                /* emit swap step */
                frame.compare_a = -1;
                frame.compare_b = -1;
                frame.swap_a    = (int)j;
                frame.swap_b    = (int)j + 1;
                frame.swaps++;
                cb(&frame, user_data);
            }
        }
    }

    /* emit done */
    frame.compare_a = -1;
    frame.compare_b = -1;
    frame.swap_a    = -1;
    frame.swap_b    = -1;
    frame.done      = 1;
    cb(&frame, user_data);
}
