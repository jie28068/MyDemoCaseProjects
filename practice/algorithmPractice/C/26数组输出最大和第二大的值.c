#include <stdio.h>

int main26()
{
	int arr[6];
	printf("请输入五个数字：\n");
	/*数据输入*/
	for (int i = 0; i < 5; i++)
	{
		scanf_s("%d", &arr[i]);
	}
	/*冒泡排序，大到小*/
	for (int j = 0; j < sizeof(arr) / sizeof(arr[0]) - 1; j++)
	{
		for (int k = 0; k < sizeof(arr) / sizeof(arr[0]) - 1 - j; k++)
		{
			if (arr[k] < arr[k + 1])
			{
				int temp = arr[k];
				arr[k] = arr[k + 1];
				arr[k + 1] = temp;
			}
		}
	}
	/*可以用递归*/
	for (int i = 0; i < 5; i++)
	{
		if (arr[i] == arr[i + 1])
		{	
			for (int j = i; j < 5-i; j++)
			{
				arr[j] = arr[j + 1];    //往前赋值			
			}
		}
	}
	int b[6];
	printf("-------去重后的数字按大到小排序--------------\n");
	for (int i = 0; i < 5; i++)
	{
		if (arr[i] != arr[i + 1]) //数组空余的地方不输出来,实际上还是存在的
		printf("%d\n", arr[i]);
	
	}
	printf("-----------------------------------------\n");
	printf("最大的数字是：%d\n", arr[0]);
	printf("第二大的数字是：%d\n", arr[1]);
	system("pause");
	return 0;
}