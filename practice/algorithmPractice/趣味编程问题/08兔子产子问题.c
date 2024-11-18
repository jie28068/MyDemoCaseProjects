#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:1 1 2 3 5 13 前面俩个数的和等于下一个数 ,典型的迭代
 */
int main08()
{
	int a = 1, b = 1, c;
	int i;
	printf("第一个月：%d\n第二个月：%d\n", a, b);//输出前俩个月的兔子
	for (i = 3; i < 30; i++)
	{
		c = a + b;
		printf("第%d个月：%d\n",i, c);
		a = b;
		b = c;
	}
	system("pause");
	return 0;
}
