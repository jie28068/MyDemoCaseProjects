#include <stdio.h>

int main42()
{
	printf("*************指针数组*************\n");
	char* arr[] = { "aaa","bbb","ccc","ddd" };
	int len = sizeof(arr) /sizeof(char *)-1;
	for (int i=0;i<len;i++)
	{
		char* temp = *(arr + i);
		*(arr + i) = *(arr + len - i);
		*(arr + len - i) = temp;
	}
	for (int i = 0; i < len+1; i++)
	{
		printf("%s\n", arr[i]);
	}
	printf("*************数组指针*************\n");
	char buf[1][10] = { "012345657" };
	char(*p)[10];
	p = buf;
	printf("%s\n", p[0]);
	system("pause");
	return 0;
}