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
int main56()
{
	int i;
	int a[10] = { 1,2,3,4,5,6,7,8,9,10 };
	int* p[10];
	for (i = 0; i < 10; i++)
	{
		p[i] = &a[i];
	}
	int** pp = p;

	int sum = 0;
	for (int j = 0; j < 10; j++)
	{
		sum += **(pp + j);
	}
	printf("%d\n", sum);
	system("pause");
	return 0;
}
