#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:1 1 2 3 5 13 ǰ���������ĺ͵�����һ���� ,���͵ĵ���
 */
int main08()
{
	int a = 1, b = 1, c;
	int i;
	printf("��һ���£�%d\n�ڶ����£�%d\n", a, b);//���ǰ�����µ�����
	for (i = 3; i < 30; i++)
	{
		c = a + b;
		printf("��%d���£�%d\n",i, c);
		a = b;
		b = c;
	}
	system("pause");
	return 0;
}
