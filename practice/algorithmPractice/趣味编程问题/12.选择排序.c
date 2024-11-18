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
int main12()
{
	int arr[] = { 10,5,63,8,45,123,95,62,4 };

	for (int i = 0; i < 9; i++)
	{	
		int index = i;
		for (int j = i+1; j < 9; j++)
		{
			if (arr[index] > arr[j])
			{
				index = j;
			}
		}
		int temp = arr[index];//在本次循环找出的最大或最小值，与循环最前面的值进行替换
		arr[index] = arr[i];
		arr[i] = temp;
	}
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf("%d \t", arr[i]);
	}
	system("pause");
	return 0;
}
