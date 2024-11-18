#include <stdio.h>

extern char* str1(char* dest, char* test);
int main13()
{
	char* p = str1("hello world", "or");
	printf("%s\n", p);
	system("pause");
	return 0;
}
char* str1(char* dest, char* test)
{
	char* p = NULL;
	char* temp = test;
	while (*dest)
	{
		p = dest;
		while (*dest == *temp)
		{
			dest++;
			temp++;
		}
		if (!*temp )
		{
			return p;
		}
		else
		{
			temp = test;
		}
		dest = p;
		dest++;
	}
	return NULL;
}
