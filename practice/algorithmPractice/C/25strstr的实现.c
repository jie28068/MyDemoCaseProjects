#include <stdio.h>

char* f25(char* p, int len)
{	
	while (1)
	{
		if (*p == 'o')
			return p;
		p++;
	}	
}


int main25()
{
	char arr[] = "hello world";
	char a = 'o';
	int len = sizeof(arr)-1;
	char* p=f25(arr, len);
	printf("%s\n", p);
	system("pause");
	return 0;
}

