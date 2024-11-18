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
void changed_p()
{
	FILE* dp = fopen("../text.txt", "r");
	FILE* p = fopen("../tex.txt", "w");
	FILE* p2 = fopen("../tex.txt", "r");
	int a = 0;
	int i = 0;
	int len = get_len(dp);
	look_p();
	printf("请输入你要修改学生的学号:");
	scanf("%d", &a);
	char d[1024] = { 0 };
	char b[1024] = { 0 };
	/*想法：由于用了删除想法二的算法，所以修改就显得很简单了，在写入临时文件时就重新赋值*/
	stu s[100] = { 0 };
	for (i = 0; i < len; i++)
	{
		fgets(d, 30, dp);//按行读出放在字符数组中
		sscanf(d, "%d %s %d %s\n", &s[i].id, s[i].name, &s[i].scores, s[i].sex);//格式化数组
		if (a == s[i].id)
		{
			printf("姓名 成绩 性别\n");
			scanf("%s %d %s", s[i].name, &s[i].scores, s[i].sex);//

		}
		fprintf(p, "%d %s %d %s\n", s[i].id, s[i].name, s[i].scores, s[i].sex);
	}
	fseek(p, 0, SEEK_SET);
	int len2 = get_len(p2);
	FILE* pp = fopen("../text.txt", "w");
	for (i = 0; i < len2; i++)
	{
		fgets(b, 30, p2);
		sscanf(b, "%d %s %d %s\n", &s[i].id, s[i].name, &s[i].scores, s[i].sex);
		fprintf(pp, "%d %s %d %s\n", s[i].id, s[i].name, s[i].scores, s[i].sex);
	}
	printf("修改成功\n");
	fclose(p);
	fclose(dp);
	fclose(p2);
	fclose(pp);
	cmback();
	system("pause");
}
