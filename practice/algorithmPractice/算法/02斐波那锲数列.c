#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:前两数字的和是后面数字的值 1 1 2 3 5 8 13 21
 */
int Fibonacci(int n)
{
	if (n <= 2)
		return 1;
	return Fibonacci(n - 1) + Fibonacci(n - 2);
}

int main02()
{
	int a = 0;
	scanf("%d", &a);
	int n = Fibonacci(a);
	printf("%d\n", n);
	system("pause");
	return 0;
}
