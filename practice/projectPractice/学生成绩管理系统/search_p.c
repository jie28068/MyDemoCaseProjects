#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "student.h"
/*
   * User:SJ
   * Time:2021/8/18
   * Function:
   * Explain:
 */
void search_p()
{
	int a = 0;
	int i = 0;
	int falg = 1;
	int b[1024] = { 0 };
	char d[1024] = { 0 };
	stu s[100] = { 0 };
	printf("请输入你要查询的学号：");
	scanf("%d", &a);
	FILE* p = fopen("../text.txt", "r");
	int len = get_len(p);
	for (i = 0; i < len; i++)
	{
		fgets(d, 30, p);
		sscanf(d, "%d %s %d %s\n", &s[i].id, s[i].name, &s[i].scores, s[i].sex);
		if (a == s[i].id)
		{
			printf("学号:%d 姓名:%s 成绩:%d 性别:%s\n", s[i].id, s[i].name, s[i].scores,s[i].sex);	
			falg = 0;
		}
	}
	if (falg)
	{
		printf("查无此人\n");
	}
	fclose(p);
	cmback();
	system("pause");
}
