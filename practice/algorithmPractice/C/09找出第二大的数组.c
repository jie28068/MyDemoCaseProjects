#include <stdio.h>
//9
int main9()
{
	int a[] = { 5,100,32,45,21,67,32,68,41,99,13,71 };
	int* p = a;
	int n = sizeof(a) / sizeof(a[0]);
	for (int i = 0; i < n; i++)
	{
		p[i] = a[i];
	}
	for (int i = 0; i < n; i++)
	{
		printf("%d\n", p[i]);
	}
	for (int i = 0; i < n-1; i++)
	{
		for (int j = 0;j<n-i-1;j++)
		{
			if (p[j] > p[j + 1])
			{
				int temp = p[j];
				p[j] = p[j + 1];
				p[j + 1] = temp;
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		printf("%d\t", p[i]);
	}
	printf("第二大数字%d\n", p[10]);
	system("pause");
	return 0;
}