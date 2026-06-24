#include "algo_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algo_interface.h"

static void print_array(const int *array, size_t size)
{
    printf("[");
    for (size_t i = 0; i < size; i++) {
        printf("%d", array[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]");
}

static void sort_callback(const SortFrame *frame, void *user_data)
{
    const char *algo = (const char *)user_data;

    if (frame->done) {
        printf("[%s] DONE — comparisons: %lu, swaps: %lu\n",
               algo, frame->comparisons, frame->swaps);
        printf("[%s] final: ", algo);
        print_array(frame->array, frame->size);
        printf("\n");
        return;
    }

    if (frame->compare_a != -1)
        printf("[%s] CMP  idx %d <-> %d | ", algo, frame->compare_a, frame->compare_b);
    if (frame->swap_a != -1)
        printf("[%s] SWAP idx %d <-> %d | ", algo, frame->swap_a, frame->swap_b);

    print_array(frame->array, frame->size);
    printf("\n");
}

int main(void)
{
    int original[] = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    size_t size = sizeof(original) / sizeof(original[0]);

    /* ── Bubble sort ── */
    int *arr1 = malloc(size * sizeof(int));
    memcpy(arr1, original, size * sizeof(int));

    printf("=== Bubble Sort ===\n");
    bubble_sort(arr1, size, sort_callback, "bubble");
    free(arr1);

    printf("\n");

    /* ── Merge sort ── */
    int *arr2 = malloc(size * sizeof(int));
    memcpy(arr2, original, size * sizeof(int));

    printf("=== Merge Sort ===\n");
    merge_sort(arr2, size, sort_callback, "merge");
    free(arr2);

    return 0;
}
