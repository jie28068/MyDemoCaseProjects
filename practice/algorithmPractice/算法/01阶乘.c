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
int factorial(int n)
{
	if (n == 1)
	{
		return 1;
	}
	return n * factorial(n - 1);
}
int main01()
{
	int a = 0;
	scanf("%d", &a);
	int n=factorial(a);
	printf("%d\n", n);
	system("pause");
	return 0;
}
