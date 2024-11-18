#include <stdio.h>

void MyStringCpy(char* dest, char* src)
{
	int i = 0;
	while (1)
	{
		*(dest + i) = *(src + i);
		if (*(src + i) == '\0')
		{
			break;
		}

		i++;
	}
}

char* My_Strcat(char* dest2 , char* src2)
{
	char* dest3 = dest2;
	while (*dest2 != '\0')
	{
		dest2++;
	}
	MyStringCpy(dest2, src2);

	return dest3;
}

int main36()
{
	char dest[30] = "5679";
	char src[] = "hello word";
	printf("合并前的字符串:%s\n%s\n", dest, src);
	char* p1 = My_Strcat(dest, src);
	printf("合并后的字符串:%s\n", p1);
	system("pause");
	return 0;	
}