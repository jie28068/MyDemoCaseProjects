#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "student.h"
/*
   * User:SJ
   * Time:2021/8/17
   * Function:�˵�����
   * Explain:
 */
enum MyEnum
{
	add=1,delete,changed,search,exita,look
}opt;
void muen()
{
star:
	printf("********ѧ������ϵͳ*******\n");
	printf("\t1.���ѧ��\n");
	printf("\t2.ɾ��ѧ��\n");
	printf("\t3.�޸�ѧ��\n");
	printf("\t4.����ѧ��\n");
	printf("\t5.�˳�\n");
	printf("6.�������ѧ����Ϣ(��Ҫ�����˺�������)\n");
	int a = 0;
	scanf("%d", &a);
	if ((a != 1) && (a != 2) &&(a != 3) && (a != 4) && (a != 5) && (a != 6))
	{
		printf("��������ȷ�Ĳ���ָ�\n");
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
