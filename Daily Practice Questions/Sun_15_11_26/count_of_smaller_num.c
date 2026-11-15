#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int val;
    int index;
} Element;

void mergeAndCount(Element* items, Element* temp, int* counts, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;
    int rightElementsSmaller = 0;

    while (i <= mid && j <= right) {
        if (items[j].val < items[i].val) {
            rightElementsSmaller++;
            temp[k++] = items[j++];
        } else {
            counts[items[i].index] += rightElementsSmaller;
            temp[k++] = items[i++];
        }
    }

    while (i <= mid) {
        counts[items[i].index] += rightElementsSmaller;
        temp[k++] = items[i++];
    }

    while (j <= right) {
        temp[k++] = items[j++];
    }

    for (int idx = left; idx <= right; idx++) {
        items[idx] = temp[idx];
    }
}

void mergeSortAndCount(Element* items, Element* temp, int* counts, int left, int right) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;
    mergeSortAndCount(items, temp, counts, left, mid);
    mergeSortAndCount(items, temp, counts, mid + 1, right);
    mergeAndCount(items, temp, counts, left, mid, right);
}


int* countSmaller(int* nums, int numsSize, int* returnSize) {
    *returnSize = numsSize;
    int* counts = (int*)calloc(numsSize, sizeof(int));
    if (numsSize == 0) {
        return counts;
    }

    Element* items = (Element*)malloc(numsSize * sizeof(Element));
    Element* temp = (Element*)malloc(numsSize * sizeof(Element));

    for (int i = 0; i < numsSize; i++) {
        items[i].val = nums[i];
        items[i].index = i;
    }

    mergeSortAndCount(items, temp, counts, 0, numsSize - 1);

    free(items);
    free(temp);

    return counts;
}

int main() {
    int nums[] = {5, 2, 6, 1};
    int size = sizeof(nums) / sizeof(nums[0]);
    int returnSize = 0;

    int* result = countSmaller(nums, size, &returnSize);

    printf("Smaller counts after self: [");
    for (int i = 0; i < returnSize; i++) {
        printf("%d%s", result[i], (i < returnSize - 1) ? ", " : "");
    }
    printf("]\n");

    free(result);

    return 0;
}