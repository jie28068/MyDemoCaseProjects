#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:  f(n-1) + n + f(n-1)
   * 1
   * 1 2 1
   * 1 2 1 3 1 2 1
   * 1 2 1 3 1 2 1 4 1 2 1 3 1 2 1
 */
int rulf(int n)
{
	if (n == 1)
	{
		return 1;
	}
	printf("%d", rulf(n - 1));
	printf("%d", n);
	printf("%d", rulf(n - 1));
}
int main03()
{
	int a = 0;
	scanf("%d", &a);
	int n = rulf(a);

	system("pause");
	return 0;
}
