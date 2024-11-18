#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:结构体中有指针，不能直接拷贝（浅拷贝）。要手动拷贝（深拷贝），先释放原有的堆空间
   * 在开辟一个堆空间，再将内容拷贝该堆空间中
 */
typedef struct opt
{
	char* name;
	int amd;
}p;
int main65()
{
	p a;
	a.name = malloc(sizeof(char)*10);
	strcpy(a.name, "aaa");
	a.amd = 10;

	p b;
	b.name = malloc(sizeof(char)*10);
	strcpy(b.name, "bbb");
	b.amd = 20;
	if (a.name)
	{
		free(a.name);
		a.name = NULL;
	}
	a.name = malloc(strlen(b.name) + 1);
	strcpy(a.name, b.name);
	a.amd = b.amd;
	printf("%s %d\n", a.name, a.amd);
	printf("%s %d\n", b.name, b.amd);
	free(b.name);
	free(a.name);
	system("pause");
	return 0;
}
