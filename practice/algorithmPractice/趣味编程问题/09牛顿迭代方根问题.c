#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:��a*x^3+b*x^2+c*x+d=0����1������xֵ
   * Explain:�� ����
 */
#include <math.h>
int main09()
{
	printf("��������ʽ��ֵ��\n");
	float a, b, c, d;
	scanf("%f %f %f %f", &a, &b, &c, &d);
	float x = 1.5, x0, f, fd, h;
	do
	{
		x0 = x;
		f = a * x0 * x0 * x0 + b * x0 * x0 + c * x0 + d;
		fd = 3 * a * x0 * x0 + 2 * b * x0 + c * x0;
		h = f / fd;
		x = x0 - h;
	} while (fabs(x - x0) >= 1e-5);
	printf("����1����Ľ�Ϊ:%f", a);
	system("pause");
	return 0;
}
