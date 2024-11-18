#include<stdio.h>

extern int add(int a, int b);
extern int sub(int a, int b);
extern int mlt(int a, int b);
extern int dive(int a, int b);

int main17()
{
	int a, b;
	char c;
	int value = 0;
	while (1)
	{

		scanf_s("%d%c%d", &a, &c, &b);
		switch (c)
		{
		case '+':
			value = add(a, b);
			printf("%d+%d=%d\n", a, b, value);
			break;
		case '-':
			value = sub(a, b);
			printf("%d-%d=%d\n", a, b, value);
			break;
		case '*':
			value = mlt(a, b);
			printf("%d*%d=%d\n", a, b, value);
			break;
		case '/':
			if (!b)
			{
				printf("0不能作为除数\n");
				continue;
			}
			value = dive(a, b);
			printf("%d/%d=%d\n", a, b, value);
			break;
		}
	}
	return 0;
}

int add(int a, int b)
{
	return a + b;
}
int sub(int a, int b)
{
	return a - b;
}
int mlt(int a, int b)
{
	return a * b;
}
int dive(int a, int b)
{
	return a / b;
}
