#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "student.h"
/*
   * User:SJ
   * Time:2021/8/17
   * Function:菜单函数
   * Explain:
 */
enum MyEnum
{
	add=1,delete,changed,search,exita,look
}opt;
void muen()
{
star:
	printf("********学生管理系统*******\n");
	printf("\t1.添加学生\n");
	printf("\t2.删除学生\n");
	printf("\t3.修改学生\n");
	printf("\t4.查找学生\n");
	printf("\t5.退出\n");
	printf("6.浏览所有学生信息(需要输入账号与密码)\n");
	int a = 0;
	scanf("%d", &a);
	if ((a != 1) && (a != 2) &&(a != 3) && (a != 4) && (a != 5) && (a != 6))
	{
		printf("请输入正确的操作指令！\n");
		getch();
		system("cls");
		goto star;
	}
	else
	{
		switch (a)
		{
		case add:
			add_p();
			break;
		case delete:
			delete_p();
			break;
		case changed:
			changed_p();
			break;
		case search:
			search_p();
			break;
		case exita:
			exit(0);
			break;
		case look:
			lookall();
			break;
		default:
			break;
		}
	}

	system("pause");
}
