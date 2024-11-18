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
	printf("请输入密码 账号\n");
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
	fclose(p);//一定要关闭写账号密码的文件P，不然不会存到文本中
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
		printf("学号 姓名 成绩 性别(男XY,女X)\n");
		while (fgets(buf, 1024, lp))
		{
			printf("%s\n", buf);
		}
		fclose(lp);
		fclose(pp);
		cmback();
	}
	else {
		printf("密码账号不对！\n");
		cmback();
	}
}
