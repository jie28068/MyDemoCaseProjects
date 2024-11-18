#include <stdio.h>
#include <Windows.h>
/*选择排序，每次循环选到最小或最大的数字放到最前面*/
int main30()
{
	int a[] = { 1,25,6,99,87,50,10 };
	for (int i = 0; i < 7; i++)
	{
		int index = i;           //作为一个标记位，去记录本次循环最小或最大的位置
		for (int j = i + 1; j < 7; j++)//循环找出最大或最小的数字
		{
			if (a[index] > a[j])
				index = j;//找到最小的位置
		}
		int temp = a[index];//在本次循环找出的最大或最小值，与循环最前面的值进行替换
		a[index] = a[i];
		a[i] = temp;
	}
	for (int i = 0; i < 7; i++)
	{
		printf("%d\t", a[i]);
	}

	system("pause");
	return 0;
}