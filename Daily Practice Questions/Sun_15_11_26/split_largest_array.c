#include <stdio.h>

int canSplit(int* nums, int numsSize, int k, int maxAllowedSum) {
    int subarrays = 1;
    int currentSum = 0;

    for (int i = 0; i < numsSize; i++) {
        if (currentSum + nums[i] > maxAllowedSum) {
            subarrays++;
            currentSum = nums[i];
            if (subarrays > k) {
                return 0;
            }
        } else {
            currentSum += nums[i];
        }
    }

    return 1;
}

int splitArray(int* nums, int numsSize, int k) {
    int left = 0;
    int right = 0;

    for (int i = 0; i < numsSize; i++) {
        if (nums[i] > left) {
            left = nums[i];
        }
        right += nums[i];
    }

    int answer = right;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (canSplit(nums, numsSize, k, mid)) {
            answer = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return answer;
}

int main() {
    int nums[] = {7, 2, 5, 10, 8};
    int size = sizeof(nums) / sizeof(nums[0]);
    int k = 2;

    int result = splitArray(nums, size, k);
    printf("Minimized largest subarray sum (k=%d): %d\n", k, result);

    return 0;
}