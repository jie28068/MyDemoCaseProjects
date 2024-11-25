#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//7
int fun7(char* p, int* n)
{
	if (p == NULL || n == NULL)
	{
		return -1;
	}
	int begin = 0;
	int end = strlen(p) - 1;
	//从左边开始
	//如果当前字符为空，而且没有结束
	while (p[begin] == ' ' && p[begin] != 0)
	{
		begin++; //位置从右移动一位
	}
	//从右往左移动
	while (p[end] == ' ' && end > 0)
	{
		end--; //往左移动
	}
	if (end == 0)
	{
		return -2;
	}
	//非空元素个数
	*n = end - begin + 1;
	return 0;
}

int main7(void)
{
	char* p = "      abcddsgadsgefg      ";
	int ret = 0;
	int n = 0;
	ret = fun7(p, &n);
	if (ret != 0)
	{
		return ret;
	}
	printf("非空字符串元素个数：%d\n", n);
	//printf("%d\n", ~1);//-2
	//printf("%d\n", ~(-1));//0
	system("pause");
	return 0;
}