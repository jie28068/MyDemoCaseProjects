#include <stdio.h>
/*
	编写一个程序，输入一个3位数的正整数，然后反向输出对应的数。
	如:123，则输出321。
*/
int main46()
{
	char buf[4] = { 0 };
	fgets(buf, sizeof(buf), stdin);
	for (int i = strlen(buf)-1; i >= 0; i--)
	{
		printf("%c", buf[i]);
	}
	printf("\n");
	system("pause");
	return 0;
}