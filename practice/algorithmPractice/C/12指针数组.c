#include <stdio.h>

int main12()
{
	char arry[] = "hello  wrold";
	printf("%d\n", sizeof(arry));
	printf("%d\n", strlen(arry));
	//��ŵ��ǵ�ַ
	char* a[] = { "hello","world","GG" };
	printf("%c\n", *(a[0]+4));//hello�����o
	printf("%p\n", &a[0]);//�׵�ַ
	printf("%d\n", strlen(a));//3
	char *b[] = {'a','b','c','d'};
	printf("%d\n",sizeof(b));//4��4�ֽڵ�ָ��16
	printf("%d\n", strlen(b));//1
	unsigned char c = 280;
	printf("%d,%u\n", c,c);
	 char d = 127;
	printf("%d,%u\n", d, d);
	system("pause");
	return 0;
}