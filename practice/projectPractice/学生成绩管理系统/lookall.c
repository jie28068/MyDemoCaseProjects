#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "student.h"
/*
   * User:SJ
   * Time:2021/8/19
   * Function:
   * Explain:
 */
void lookall()
{	
	printf("���������� �˺�\n");
	char b[100] = { 0 };
	stu s;
	int id;
	char name[20];
	scanf("%d %s", &id, name);
	FILE* p = fopen("../ami.txt", "w+");
	FILE* pp = fopen("../ami.txt", "r");
	fprintf(p, "%d %s\n",123456,"user");
	fseek(p, 0, SEEK_SET);
	fgets(b, 30, p);
	fclose(p);//һ��Ҫ�ر�д�˺�������ļ�P����Ȼ����浽�ı���
	sscanf(b, "%d %s", &s.id, s.name);
	if (id == s.id && (!strcmp(name,s.name)))
	{
		FILE* lp = fopen("../text.txt", "r+");
		if (NULL == lp)
			return;
		int len = get_len(lp);
		int i = 0;
		int a = 0;
		char buf[1024] = { 0 };
		printf("ѧ�� ���� �ɼ� �Ա�(��XY,ŮX)\n");
		while (fgets(buf, 1024, lp))
		{
			printf("%s\n", buf);
		}
		fclose(lp);
		fclose(pp);
		cmback();
	}
	else {
		printf("�����˺Ų��ԣ�\n");
		cmback();
	}
}
