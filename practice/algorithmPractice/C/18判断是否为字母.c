#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main18()
{
	char str[100];
	char*p = str;
end:
	printf("�������ַ���\n");
	fgets(str, sizeof(str), stdin);
	while (1)
	{

		if (strlen(str) == 2)
		{	

			if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))
			{
				printf("yes\n");
				goto end;
			}
			else if (*p == '#')
			{
				break;
			}
			else
			{
				printf("no\n");
				goto end;
			}
		}
		else
		{
			printf("����������һ���ַ�\n");
			goto end;
		}

	}

	system("pause");
	return 0;	
}