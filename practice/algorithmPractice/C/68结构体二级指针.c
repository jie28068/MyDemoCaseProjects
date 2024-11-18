#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:把内存结构图画好，分析好数据类型
 */
typedef struct two
{
	char* tearch;
	char** student;
}opt;
//创建内存
int changed68(opt***p)
{
	if (NULL == p)
		return -1;
	opt** ns = malloc(sizeof(opt*) * 3);
	for (int i = 0; i < 3; i++)
	{
		ns[i] = malloc(sizeof(opt));
		ns[i]->tearch = malloc(sizeof(char) * 64);
		sprintf(ns[i]->tearch, "Teacher_%d", i + 1);
		ns[i]->student = malloc(sizeof(char*) * 4);
		for (int j = 0; j < 4; j++)
		{
			ns[i]->student[j] = malloc(sizeof(char) * 64);
			sprintf(ns[i]->student[j], "%s_Stu_%d", ns[i]->tearch, j + 1);
		}
	}
	*p = ns;
	return 0;
}
//打印
void pri68(opt**p)
{
	for (int i = 0; i < 3; i++)
	{	
		for (int j = 0; j < 4; j++)
		{
			printf("%s %s\n", p[i]->tearch, p[i]->student[j]);
		}
		printf("\n");
	}
}
//释放内存
void fere68(opt**p)
{
	for (int i = 0; i < 3; i++)
	{
		free(p[i]->tearch);
		p[i]->tearch = NULL;
		for (int j = 0; j < 4; j++)
		{
			free(p[i]->student[j]);
			p[i]->student[j] = NULL;
		}
		free(p[i]->student);
		p[i]->student = NULL;
		free(p[i]);
		p[i] = NULL;
	}
}
int main68()
{
	opt** p = NULL;
	int ren=changed68(&p);
	if (ren < 0)
	{
		printf("创建内存失败!\n");
	}
	pri68(p);
	fere68(p);
	system("pause");
	return 0;
}
