#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
void swap(int* a, int* b) {
    int t = *a;
    *a = *b, * b = t;
}

void reverse(int* nums, int start, int end) {
    while (start < end) {
        swap(&nums[start], &nums[end]);
        start += 1;
        end -= 1;
    }
}

void rotate(int* nums, int numsSize, int k) {
    k %= numsSize;
    reverse(nums, 0, numsSize - 1);
    reverse(nums, 0, k - 1);
    reverse(nums, k, numsSize - 1);
}

int main06()
{
    int arr[] = { 1,2,3,4,5,6,7 };
    rotate(arr, 7, 3);
    for (int i = 0; i < 7; ++i)
    {
        printf("%d\t", arr[i]);
    }
    system("pause");
    return 0;
}
#endif