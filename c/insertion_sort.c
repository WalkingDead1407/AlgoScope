#include "algo_interface.h"

void insertion_sort(int *array, size_t size, SortCallback cb, void *user_data)
{
    SortFrame frame = {0};
    frame.array = array;
    frame.size = size;
    frame.compare_a = frame.compare_b = -1;
    frame.swap_a = frame.swap_b = -1;

    for (size_t i = 1; i < size; i++) {
        int key = array[i];
        size_t j = i;

        while (j > 0) {
            frame.compare_a = (int)(j - 1);
            frame.compare_b = (int)i;
            frame.swap_a = frame.swap_b = -1;
            frame.comparisons++;
            cb(&frame, user_data);

            if (array[j - 1] <= key)
                break;

            array[j] = array[j - 1];
            frame.compare_a = frame.compare_b = -1;
            frame.swap_a = (int)j;
            frame.swap_b = (int)(j - 1);
            frame.swaps++; /* Counts each shift/write operation. */
            cb(&frame, user_data);
            j--;
        }

        if (array[j] != key) {
            array[j] = key;
            frame.compare_a = frame.compare_b = -1;
            frame.swap_a = (int)j;
            frame.swap_b = -1;
            frame.swaps++;
            cb(&frame, user_data);
        }
    }

    frame.compare_a = frame.compare_b = -1;
    frame.swap_a = frame.swap_b = -1;
    frame.done = 1;
    cb(&frame, user_data);
}
