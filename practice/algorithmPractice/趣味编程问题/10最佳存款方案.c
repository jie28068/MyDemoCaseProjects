#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:��Ϣ0.0063��ÿ��ȡ1000ȡ����ȡ�꣬��ԭ�����˶��ٽ�ȥ
 */
int main10()
{
	float pum = 0.0063;
	double x = 0.0;
	for (int i = 0; i < 5; i++)
	{
		x = (x + 1000.0) / (1 + 12 * 0.0063);
	}
	printf("��ϢΪ:%.3lf\n",x);
	system("pause");
	return 0;
}
