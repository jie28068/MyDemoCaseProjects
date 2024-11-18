#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#define mad(a,b) (a+b)//宏函数，但是不是真正的函数
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:真正的函数三要素：参数，返回值，函数体
 */
int fun57(int a,int b)
{
	int t_a = a;
	int T_b = b;
	int c = t_a + T_b;
	return c;//返回值放在寄存器中
}
void func57()
{
	char a[] = "hello";
	char b[] = "world";
	for (int i = 0; i < strlen(a); i++)
	{
		a[i] = a[i] ^ b[i];
		b[i] = a[i] ^ b[i];
		a[i] = a[i] ^ b[i];
	}

	printf("%s\n", a);
	printf("%s\n", b);
}
int main57()
{
	//char* p = "abcdefgh ", * r;
	//long* q;
	//q = (long*)p;
	//q++;
	//r = (char*)q;
	////r++;//我本来就是char类型的，加加后当然就是移动一个位置
	//printf("%s", r);
	//printf("%d\n", mad(3, 4));
	int a = 10;
	int b = 20;
	int c = 0;
	 c = fun57(a, b);
	printf("%d\n", c);
	system("pause");
	return 0;
}