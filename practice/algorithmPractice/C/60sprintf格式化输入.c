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
#if 0
void test60() {

	//1. ��ʽ���ַ���
	char buf[1024] = { 0 };
	sprintf(buf, "���,%s,��ӭ��������!", "John");
	printf("buf:%s\n", buf);
	memset(buf, 0, 1024);
	sprintf(buf, "�ҽ���%d����!", 20);
	printf("buf:%s\n", buf);

	//2. ƴ���ַ���
	memset(buf, 0, 1024);
	char str1[] = "hello";
	char str2[] = "world";
	int len = sprintf(buf, "%s %s", str1, str2);
	printf("buf:%s len:%d\n", buf, len);

	//3. ����ת�ַ���
	memset(buf, 0, 1024);
	int num = 100;
	sprintf(buf, "%d", num);
	printf("buf:%s\n", buf);
	//���ÿ�� �Ҷ���
	memset(buf, 0, 1024);
	sprintf(buf, "%8d", num);
	printf("buf:%s\n", buf);
	//���ÿ�� �����
	memset(buf, 0, 1024);
	sprintf(buf, "%-8d", num);
	printf("buf:%s\n", buf);
	//ת��16�����ַ��� Сд
	memset(buf, 0, 1024);
	sprintf(buf, "0x%x", num);
	printf("buf:%s\n", buf);

	//ת��8�����ַ���
	memset(buf, 0, 1024);
	sprintf(buf, "0%o", num);
	printf("buf:%s\n", buf);
}

//1. ��������
void test12() {
	char buf[1024] = { 0 };
	//����ǰ�������
	//ƥ���һ���ַ��Ƿ������֣�����ǣ�������
	//���������ֹͣƥ��
	sscanf("123456aaaa", "%*d%s", buf);
	printf("buf:%s\n", buf);
}

//2. ��ȡָ���������
void test11() {
	char buf[1024] = { 0 };
	//����ǰ�������
	sscanf("123456aaaa", "%7s", buf);
	printf("buf:%s\n", buf);
}

//3. ƥ��a-z�������ַ�
void test03() {
	char buf[1024] = { 0 };
	//����ǰ�������
	//��ƥ���һ���ַ����ж��ַ��Ƿ���a-z�е��ַ��������ƥ��
	//�������ֹͣƥ��
	sscanf("abcdefg123456", "%[a-z]", buf);
	printf("buf:%s\n", buf);
}

//4. ƥ��aBc�е��κ�һ��
void test04() {
	char buf[1024] = { 0 };
	//����ǰ�������
	//��ƥ���һ���ַ��Ƿ���aBc�е�һ��������ǣ���ƥ�䣬���������ֹͣƥ��
	sscanf("abcdefg123456", "%[aBc]", buf);
	printf("buf:%s\n", buf);
}

//5. ƥ���a�������ַ�
void test05() {
	char buf[1024] = { 0 };
	//����ǰ�������
	//��ƥ���һ���ַ��Ƿ���aBc�е�һ��������ǣ���ƥ�䣬���������ֹͣƥ��
	sscanf("bcdefag123456", "%[^a]", buf);
	printf("buf:%s\n", buf);
}

//6. ƥ���a-z�е������ַ�
void test06() {
	char buf[1024] = { 0 };
	//����ǰ�������
	//��ƥ���һ���ַ��Ƿ���aBc�е�һ��������ǣ���ƥ�䣬���������ֹͣƥ��
	sscanf("123456ABCDbcdefag", "%[^a-z]", buf);
	printf("buf:%s\n", buf);
}

int main60()
{
	test11();

	system("pause");
	return 0;
}
#endif