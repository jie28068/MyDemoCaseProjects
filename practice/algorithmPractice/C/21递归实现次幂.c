#include <stdio.h>

extern double func25(double a, int b);
extern double func251(double a, int b);
int main21()
{
	double a;
	int b;
	double c;
	scanf_s("%lf%d", &a, &b);
	if (b >= 0)
	{
		c = func25(a, b);
	}
	if (b < 0)
	{
		c = func251(a, -b);
	}
	printf("%.2lf\n", c);

	system("pause");
	return 0;
}

double func25(double a, int b)
{
	if (b == 0)
		return 1;
	if (b < 2)
		return a;
	return func25(a, b - 1) * a;
}
double func251(double a, int b)
{
	if (b < 2)
		return 1 / a;
	return func25(a, b - 1) * (1 / a);
}