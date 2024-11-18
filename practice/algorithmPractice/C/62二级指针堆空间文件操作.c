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
int func_len(FILE* file_p)//�õ��ļ����ַ���������
{
	if (NULL == file_p)
	{
		return;
	}
	char buf[1024] = { 0 };
	int len=0;
	while (fgets(buf, 1024, file_p))
	{
		++len;
	}
	fseek(file_p, 0, SEEK_SET);//�ص��ļ���ͷλ��
	return len;
}
void func_put(char **p,int len, FILE* file_p)
{
	if (NULL == file_p)
	{
		return;
	}
	if (NULL == p)
	{
		return;
	}
	if (len <= 0)
	{
		return;
	}
	char buf[1024] = { 0 };
	int index = 0;
	while (fgets(buf, 1024, file_p) != NULL)
	{
		int string_len = strlen(buf) + 1;
		char* count_p = malloc(sizeof(char) * string_len);
		if (NULL == count_p)
		{
			return;
		}
		strcpy(count_p, buf);//���ļ������ݰ��п�����һ��ָ��
		p[index++] = count_p;//��һ��ָ������ݸ�����ָ��
	}
}
void func_show(char** func_p, int len)
{
	for (int i = 0; i < len; i++)
	{
		printf("%s", func_p[i]);
	}
}
void func_free(char** p,int len)//�ͷŶѿռ�
{
	for (int i = 0; i < len; i++)
	{
		if (p[i] != NULL)
		{
			free(p[i]);
			p[i] = NULL;
		}
	}
	free(p);
	p = NULL;
}
int main62()
{
	FILE* file_p = fopen("../text.txt", "r");
	int len=func_len(file_p);
	printf("�ļ�������%d\n", len);
	char** func_p = malloc(sizeof(char*) * len);//����ָ��ѿռ�
	func_put(func_p, len,file_p);
	func_show(func_p, len);
	func_free(func_p, len);
	system("pause");
	return 0;
}
#endif
