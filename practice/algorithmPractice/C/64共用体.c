#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
����union��һ������ͬһ���洢�ռ�洢��ͬ�������ݵ����ͣ�
��������ռ���ڴ泤�ȵ��������Ա�ĳ��ȣ�Ҳ�н��������壻
ͬһ�ڴ�ο���������ż��ֲ�ͬ���͵ĳ�Ա����ÿһ˲ʱֻ��һ�������ã�
����������������õĳ�Ա�����һ�δ�ŵĳ�Ա���ڴ���һ���µĳ�Ա��ԭ�еĳ�Ա��ֵ�ᱻ���ǣ�
����������ĵ�ַ�����ĸ���Ա�ĵ�ַ����ͬһ��ַ��
   * Explain:
 */
 //������Ҳ�������� 
union Test
{
	unsigned char a;
	unsigned int b;
	unsigned short c;
};

int main64()
{
	//���干�������
	union Test tmp;

	//1�����г�Ա���׵�ַ��һ����
	printf("%p, %p, %p\n", &(tmp.a), &(tmp.b), &(tmp.c));

	//2���������СΪ����Ա���͵Ĵ�С
	printf("%lu\n", sizeof(union Test));

	//3��һ����Ա��ֵ����Ӱ������ĳ�Ա
	//����Ǹ�λ���ұ��ǵ�λ
	//��λ�ŵ͵�ַ����λ�Ÿߵ�ַ
	tmp.b = 0x44332211;

	printf("%x\n", tmp.a); //11
	printf("%x\n", tmp.c); //2211

	tmp.a = 0x00;
	printf("short: %x\n", tmp.c); //2200
	printf("int: %x\n", tmp.b); //44332200
	system("pause");
	return 0;
}