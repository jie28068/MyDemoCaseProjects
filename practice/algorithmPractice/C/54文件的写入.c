#include <stdio.h>
//#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdlib.h>
int main54()
{
	char name[256];
	scanf("%s", name);
	getchar();
	FILE* p=fopen(name, "w");
	if (!p)
	{
		return -1;
	}
	char buf[1024];
	while (1)
	{
		fgets(buf, 1024, stdin);
		if (!strncmp(buf, "exit", 4))
			break;
		fputs(buf, p);

	}
	fclose(p);
	system("pause");
	return 0;
}