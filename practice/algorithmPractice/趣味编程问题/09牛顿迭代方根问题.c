#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:求a*x^3+b*x^2+c*x+d=0，在1附近的x值
   * Explain:求导 切数
 */
#include <math.h>
int main09()
{
	printf("请输入表达式的值！\n");
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
	printf("距离1最近的解为:%f", a);
	system("pause");
	return 0;
}
