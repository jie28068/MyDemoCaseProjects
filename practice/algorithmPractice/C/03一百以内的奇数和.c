#include <stdio.h>

int main3()
{
	int  sum = 0;
	for (int i = 1; i <=99; i += 2)
	{		
		sum = sum + i;
		printf_s("%d\n", i);		
	}
	printf_s("%d\n", sum);
	system("pause");
}