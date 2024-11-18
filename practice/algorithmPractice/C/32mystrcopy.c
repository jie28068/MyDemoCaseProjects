#include <stdio.h>

char* My_StrCopy(char* dest, char* src, int len)
{
	char* p = src;
	for (int i = 0; i < len; i++)
	{
		p[i] = src[i];
	}
	for (int i = 0; i < len; i++)
	{
		dest[i] = p[i];
	}	
	return dest;
}



int main32()
{
	char dest[20] = "123456789";
	char src[] = "hello world";
	int len = sizeof(src) / sizeof(src[0]);
	printf("复制前：%s\n", dest);
	char* p=My_StrCopy(dest, src,len);
	printf("复制后：%s\n", p);
	system("pause");
	return 0;
}