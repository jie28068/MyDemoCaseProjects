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
		int temp = arr[index];//�ڱ���ѭ���ҳ���������Сֵ����ѭ����ǰ���ֵ�����滻
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
