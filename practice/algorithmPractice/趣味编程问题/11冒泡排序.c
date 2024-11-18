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
int main11()
{
	int arr[] = { 10,5,63,8,45,123,95,62,4 };
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]) - 1; i++)
	{
		for (int j = 0; j < sizeof(arr) / sizeof(arr[0]) - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				arr[j] = arr[j] ^ arr[j + 1];
				arr[j+1] = arr[j] ^ arr[j + 1];
				arr[j] = arr[j] ^ arr[j + 1];
			}
		}
	}
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf("%d \t", arr[i]);
	}
	system("pause");
	return 0;
}
