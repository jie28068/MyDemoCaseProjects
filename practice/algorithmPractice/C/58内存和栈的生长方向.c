#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
 //1. 栈的生长方向
void test01() {

	int a = 10;
	int b = 20;
	int c = 30;
	int d = 40;

	printf("a = %p\n", &a);
	printf("b = %p\n", &b);
	printf("c = %p\n", &c);
	printf("d = %p\n", &d);

	//a的地址大于b的地址，故而生长方向向下
}
//2. 内存生长方向(小端模式)
void test02() {

	//高位字节 -> 低位字节
	int num = 0xaabbccdd;
	unsigned char* p = &num;

	//从首地址开始的第一个字节
	printf("%x\n", *p);
	printf("%x\n", *(p + 1));
	printf("%x\n", *(p + 2));
	printf("%x\n", *(p + 3));
}
int main58()
{
	int a = 10;
	int b = 20;
	test01();
	test02();
	system("pause");
	return 0;
}
