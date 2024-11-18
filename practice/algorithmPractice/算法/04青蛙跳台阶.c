#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:每次可以跳1或者2台阶，那么跳到N阶台阶，有多少种方法
 */
int func04(n)
{
	int a = -1;
	int* arr=&a;
	if (n <= 2)//把N压缩到很小，得到递归结束条件
	{
		return n;
	}
	//先判断有没计算过
	if (arr[n] != -1) {
		//计算过，直接返回
		return arr[n];
	}
	else {
		// 没有计算过，递归计算,并且把结果保存到 arr数组里
		arr[n] = func04(n - 1) + func04(n - 1);
		return arr[n];
	}
}
int main04()
{
	int a = 0;
	scanf("%d", &a);
	int n = func04(a);
	printf("%d\n", n);
	system("pause");
	return 0;
}
