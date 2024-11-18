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

void select5(int *a)
{
	int key;
	/*0*/
	int min = 0;
	/*中间值*/
	int mid = 0;
	/*数组长度*/
	int max = 5;
	scanf_s("%d", &key);
	int flag = 0;
	while (min <= max)
	{
		mid = (min + max) / 2;
		/*输入的值大于中间值时，重新对min赋值*/
		if (key > a[mid])
		{
			min = mid + 1;
		}
		/*输入的值小于中间值时，重新对max赋值*/
		else if (key < a[mid])
		{
			max = mid - 1;
		}
		else
		{
			printf("%d,%d", a[mid], mid);
			flag = 1;
			break;
		}
	}
	if (0 == flag)
	{
		printf("没有这个数字\n");
	}
}
int main5()
{
	int a[5];
	for (int i = 0; i < 5; ++i)
	{
		scanf("%d", &a[i]);
	}
	select5(a);
	system("pause");
	return 0;
}
