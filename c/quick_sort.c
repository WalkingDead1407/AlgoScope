#include "algo_interface.h"

/* Lomuto partition: the last element is the pivot. */
static size_t partition(int *array, size_t low, size_t high,
                        SortFrame *frame, SortCallback cb, void *user_data)
{
    int pivot = array[high];
    size_t i = low;

    for (size_t j = low; j < high; j++) {
        frame->compare_a = (int)j;
        frame->compare_b = (int)high;
        frame->swap_a = -1;
        frame->swap_b = -1;
        frame->comparisons++;
        cb(frame, user_data);

        if (array[j] <= pivot) {
            if (i != j) {
                int tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
                frame->compare_a = -1;
                frame->compare_b = -1;
                frame->swap_a = (int)i;
                frame->swap_b = (int)j;
                frame->swaps++;
                cb(frame, user_data);
            }
            i++;
        }
    }

    if (i != high) {
        int tmp = array[i];
        array[i] = array[high];
        array[high] = tmp;
        frame->compare_a = -1;
        frame->compare_b = -1;
        frame->swap_a = (int)i;
        frame->swap_b = (int)high;
        frame->swaps++;
        cb(frame, user_data);
    }

    return i;
}

static void quick_sort_recursive(int *array, size_t low, size_t high,
                                 SortFrame *frame, SortCallback cb, void *user_data)
{
    if (low >= high) return;

    size_t pivot = partition(array, low, high, frame, cb, user_data);
    if (pivot > low)
        quick_sort_recursive(array, low, pivot - 1, frame, cb, user_data);
    if (pivot < high)
        quick_sort_recursive(array, pivot + 1, high, frame, cb, user_data);
}

void quick_sort(int *array, size_t size, SortCallback cb, void *user_data)
{
    SortFrame frame = {0};
    frame.array = array;
    frame.size = size;
    frame.compare_a = frame.compare_b = -1;
    frame.swap_a = frame.swap_b = -1;

    if (size > 1)
        quick_sort_recursive(array, 0, size - 1, &frame, cb, user_data);

    frame.compare_a = frame.compare_b = -1;
    frame.swap_a = frame.swap_b = -1;
    frame.done = 1;
    cb(&frame, user_data);
}
