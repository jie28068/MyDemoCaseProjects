#include <stdio.h>

int main12()
{
	char arry[] = "hello  wrold";
	printf("%d\n", sizeof(arry));
	printf("%d\n", strlen(arry));
	//存放得是地址
	char* a[] = { "hello","world","GG" };
	printf("%c\n", *(a[0]+4));//hello里面的o
	printf("%p\n", &a[0]);//首地址
	printf("%d\n", strlen(a));//3
	char *b[] = {'a','b','c','d'};
	printf("%d\n",sizeof(b));//4个4字节的指针16
	printf("%d\n", strlen(b));//1
	unsigned char c = 280;
	printf("%d,%u\n", c,c);
	 char d = 127;
	printf("%d,%u\n", d, d);
	system("pause");
	return 0;
}