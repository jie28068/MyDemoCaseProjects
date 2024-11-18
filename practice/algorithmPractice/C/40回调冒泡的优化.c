#include <stdio.h>
#include <stdbool.h>

typedef bool (*p)(int, int);
bool setup(int a,int b)//升序
{
	if (a > b)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool setdown(int a, int b)//降序
{
	if (a < b)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void bubble(bool (*p)(int,int),int* arr,int len)//实现了接口的封装
{
	for (int i = 0; i < len - 1; i++)
	{
		for (int j = 0; j < len - 1 - i; j++)
		{
			if (p(arr[j], arr[j + 1])) //回调函数
			{
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

int main40()
{
	int arr[5] = { 25,98,5,41,233 };
	int len = sizeof(arr) / sizeof(arr[0]);
	bubble(setup, arr, len);
	for (int i = 0; i < len; i++)
	{
		printf("%d\t", arr[i]);
	}
	system("pause");
	return 0;
}