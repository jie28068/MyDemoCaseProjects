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
 //1. ջ����������
void test01() {

	int a = 10;
	int b = 20;
	int c = 30;
	int d = 40;

	printf("a = %p\n", &a);
	printf("b = %p\n", &b);
	printf("c = %p\n", &c);
	printf("d = %p\n", &d);

	//a�ĵ�ַ����b�ĵ�ַ���ʶ�������������
}
//2. �ڴ���������(С��ģʽ)
void test02() {

	//��λ�ֽ� -> ��λ�ֽ�
	int num = 0xaabbccdd;
	unsigned char* p = &num;

	//���׵�ַ��ʼ�ĵ�һ���ֽ�
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
