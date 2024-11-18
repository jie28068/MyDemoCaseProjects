#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:给定一个整数数组 nums 和一个整数目标值 target，请你在该数组中找出和为目标值 target
	  的那两个整数，并返回它们的数组下标。你可以假设每种输入只会对应一个答案。但是，数组中同一个
	  元素在答案里不能重复出现。
	  T(n)=O(n^2)
 */
int* func01(int* arr, int size, int target)
{
	int* returnarr = NULL;
	for (int i = 0; i < size; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if (arr[i] + arr[j] == target)
			{
				returnarr = malloc(sizeof(int) * 2);
				if (NULL == returnarr)
					return;
				returnarr[0] = i;
				returnarr[1] = j;
				return returnarr;
			}
		}
	}
	return returnarr;
}

int main()
{
	int arr[5];
	for (int i = 0; i < 5; i++)
	{
		scanf("%d", &arr[i]);
	}
	int* p = func01(arr, 5, 9);
	printf("%d %d\n", p[0], p[1]);
	system("pause");
	return 0;
}
#endif