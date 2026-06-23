#include "algo_interface.h"
#include <stdlib.h>
#include <string.h>

static void merge(int *array, size_t left, size_t mid, size_t right,
                  SortFrame *frame, SortCallback cb, void *user_data)
{
    size_t left_size  = mid - left;
    size_t right_size = right - mid;

    int *left_buf  = malloc(left_size  * sizeof(int));
    int *right_buf = malloc(right_size * sizeof(int));

    memcpy(left_buf,  array + left, left_size  * sizeof(int));
    memcpy(right_buf, array + mid,  right_size * sizeof(int));

    size_t i = 0, j = 0, k = left;

    while (i < left_size && j < right_size) {

        /* emit compare */
        frame->compare_a = (int)(left + i);
        frame->compare_b = (int)(mid  + j);
        frame->swap_a    = -1;
        frame->swap_b    = -1;
        frame->comparisons++;
        cb(frame, user_data);

        if (left_buf[i] <= right_buf[j]) {
            array[k] = left_buf[i];
            i++;
        } else {
            array[k] = right_buf[j];
            j++;
        }

        /* emit write-back */
        frame->compare_a = -1;
        frame->compare_b = -1;
        frame->swap_a    = (int)k;
        frame->swap_b    = -1;
        frame->swaps++;
        cb(frame, user_data);

        k++;
    }

    while (i < left_size) {
        array[k] = left_buf[i];

        /* emit write-back */
        frame->compare_a = -1;
        frame->compare_b = -1;
        frame->swap_a    = (int)k;
        frame->swap_b    = -1;
        frame->swaps++;
        cb(frame, user_data);

        i++; k++;
    }

    while (j < right_size) {
        array[k] = right_buf[j];

        /* emit write-back */
        frame->compare_a = -1;
        frame->compare_b = -1;
        frame->swap_a    = (int)k;
        frame->swap_b    = -1;
        frame->swaps++;
        cb(frame, user_data);

        j++; k++;
    }

    free(left_buf);
    free(right_buf);
}

static void merge_sort_recursive(int *array, size_t left, size_t right,
                                  SortFrame *frame, SortCallback cb, void *user_data)
{
    if (right - left <= 1)
        return;

    size_t mid = left + (right - left) / 2;

    merge_sort_recursive(array, left, mid,   frame, cb, user_data);
    merge_sort_recursive(array, mid,  right, frame, cb, user_data);
    merge(array, left, mid, right, frame, cb, user_data);
}

void merge_sort(int *array, size_t size, SortCallback cb, void *user_data)
{
    SortFrame frame;
    frame.array       = array;
    frame.size        = size;
    frame.compare_a   = -1;
    frame.compare_b   = -1;
    frame.swap_a      = -1;
    frame.swap_b      = -1;
    frame.comparisons = 0;
    frame.swaps       = 0;
    frame.done        = 0;

    merge_sort_recursive(array, 0, size, &frame, cb, user_data);

    /* emit done */
    frame.compare_a = -1;
    frame.compare_b = -1;
    frame.swap_a    = -1;
    frame.swap_b    = -1;
    frame.done      = 1;
    cb(&frame, user_data);
}
