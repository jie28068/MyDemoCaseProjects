#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "student.h"
void delete_p()
{
	FILE* dp = fopen("../text.txt", "r");
	FILE* p = fopen("../tex.txt", "w");
	FILE* p2 = fopen("../tex.txt", "r");
	int a = 0;
	int i = 0;
	int len = get_len(dp);
	look_p();
	printf("请输入你要删除学生的学号:");
	scanf("%d", &a);
	char d[1024] = { 0 };
	char b[1024] = { 0 };
	/*想法1：先将每行的数据拿出并保存到一个字符数组中，
	再将每行的第一个字符与输入的数据比较，相同就把这一行置空*/
	/*想法2：比较每行的第一个数据，重新生成一个文件，将除要删除的不写入，其他的都写入
	然后再把这个临时文件替换掉原有的文件（可行）*/
	stu s[100] = {0};
	for (i = 0; i < len; i++)
	{
		//s[i].id = malloc(sizeof(int));
		fgets(d, 30, dp);//按行读出放在字符数组中
		sscanf(d, "%d %s %d %s\n", &s[i].id, s[i].name, &s[i].scores,s[i].sex);//格式化数组
		//fscanf(dp, "%d %s %.1f", &s[i].id ,s[i].name ,&s[i].scores);		
		//printf("学号：%d 姓名：%s 成绩：%.1f\n", s[i].id, s[i].name, s[i].scores);	
		if (a == s[i].id)
		{
			continue;
		}
		else
		{
			fprintf(p, "%d %s %d %s\n", s[i].id, s[i].name, s[i].scores, s[i].sex);
		}
	}
	//fputs(d, pp);
	fseek(p, 0, SEEK_SET);//写完最后一个文件时，光标在最后面，要把他放到开头不然行数读不出
	int len2 = get_len(p2);
	FILE* pp = fopen("../text.txt", "w");
	for (i = 0; i < len2; i++)
	{
		fgets(b, 30, p2);
		sscanf(b, "%d %s %d %s\n", &s[i].id, s[i].name, &s[i].scores, s[i].sex);
		fprintf(pp, "%d %s %d %s\n", s[i].id, s[i].name, s[i].scores, s[i].sex);
	}
	printf("删除成功\n");
	fclose(p);
	fclose(dp);
	fclose(p2);
	fclose(pp);
	cmback();
	system("pause");
}