#include <stdio.h>

int findMin(int* nums, int numsSize) {
    int left = 0;
    int right = numsSize - 1;

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (nums[mid] > nums[right]) {
            left = mid + 1;
        } else if (nums[mid] < nums[right]) {
            right = mid;
        } else {
            right--;
        }
    }

    return nums[left];
}

int main() {
    int nums[] = {2, 2, 2, 0, 1};
    int size = sizeof(nums) / sizeof(nums[0]);

    int minimum = findMin(nums, size);
    printf("Minimum element: %d\n", minimum);

    return 0;
}