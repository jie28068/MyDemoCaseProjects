#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:ÿ�ο�����1����2̨�ף���ô����N��̨�ף��ж����ַ���
 */
int func04(n)
{
	int a = -1;
	int* arr=&a;
	if (n <= 2)//��Nѹ������С���õ��ݹ��������
	{
		return n;
	}
	//���ж���û�����
	if (arr[n] != -1) {
		//�������ֱ�ӷ���
		return arr[n];
	}
	else {
		// û�м�������ݹ����,���Ұѽ�����浽 arr������
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
