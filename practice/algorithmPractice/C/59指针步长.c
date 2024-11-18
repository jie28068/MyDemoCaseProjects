#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <stddef.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
struct MyStruct
{
	char a;
	int c;
	double f[10];
};
int main59()
{
	char buf[1024] = { 0 };
	int a = 100;
	memcpy(buf + 2, &a,sizeof(int));
	char* p = buf;
	printf("%d\n", *(int*)(p + 2));//要取什么类型的数据就强转成什么样的类型
	printf("%d\n",buf[2]);
	printf("%d\n", offsetof(struct MyStruct , a));
	system("pause");
	return 0;
}
