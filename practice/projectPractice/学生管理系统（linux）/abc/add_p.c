//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <Windows.h>
#include "student.h"
/*
   * User:SJ
   * Time:2021/8/17
   * Function:
   * Explain:
 */
void add_p()
{
	FILE* p = fopen("../text.txt", "a+");
	FILE* fp = fopen("../text.txt", "r");
	if (NULL == fp)
	{
		return;
	}
	int Max=get_len(fp);//获取文件行数
	int b[1024] = {0};
	char d[1024] = {0};
	int i = 0;
	int a = 0;
	if (NULL == p)
	{
		return;
	}
	stu *s=(stu*)malloc(sizeof(stu));
	if (NULL == s)
	{
		return;
	}
star_p:
	printf("请输入学号  姓名  成绩 性别（XY/X）\n");
	//scanf("%d %s %.1f", &(s->id), s->name, &(s->scores));
	scanf("%d %s %d %s", &(s->id), s->name, &(s->scores) ,s->sex);
	while (fgets(d, 30, fp))//逐行的取出txt中的内容
	{	//d的地址都是一样的,不能用指针数组
		b[i++] =get_once(d);//将每一行的第一个字符取出	
	}
	//int n = 0;
	//sscanf(b, "%d", &n);//将字符数组转换为int类型
	/*防止有重复的学号*/
	for (i = 0; i < Max; i++)
	{
		if ((s->id) == b[i])
		{
			printf("该学生已存在！请重新输入！\n");
			goto star_p;
		}
	}
	fprintf(p, "%d %s %d %s \n", s->id, s->name, s->scores, s->sex);
	printf("录入成功\n");
	free(s);
	fclose(p);
	fclose(fp);
	cmback();
	//system("pause");
}
