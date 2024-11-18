#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <stdbool.h>
#define size 1024
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
typedef int datapef;
//����ջ�Ľṹ��
typedef struct mystack
{
	int arr[size];//�����鷽ʽ���
	int top;//�����±�
}ss;
//��ʼ��ջ
ss* changed_stack()
{
	ss* sp = malloc(sizeof(ss));
	if (NULL == sp)
		return 0;
	//��ʼ��ջ
	sp->top = -1;
	return sp;
}
//����ʼ��
void free_stack(ss* sp)
{
	if (NULL != sp)
	{
		free(sp);
		sp = NULL;
	}
}
//���ջ
void clear_stack(ss* sp)
{
	if (sp)
	{
		sp->top = -1;
	}
}
//�ж�ջ�Ƿ�Ϊ��
int empty_stack(ss* sp)
{
	if (sp)
	{
		return sp->top == -1;
	}
	return -1;
}
//��ȡջ�ĳ���
int len_stack(ss* sp)
{
	if (sp)
	{
		return sp->top + 1;
	}
	return -1;
}
//��ȡջ��Ԫ��
datapef getup_stcak(ss* sp)
{
	if (sp)
	{
		return sp->arr[sp->top];
	}
	return -1;
}
//��ջ
datapef pop_stack(ss* sp)
{
	if (sp)
	{
		return sp->arr[sp->top--];
	}
	return -1;
}
//��ջ
bool push_stack(ss* sp, datapef arr)
{
	if (sp)
	{
		sp->arr[++sp->top] = arr;
		return true;
	}
	return false;
}
//ʮ����ת�˽���
int convert(ss* sp,int n)
{
	int temp;
	if (n == 0)
	{	
		printf("�˽��ƣ�o");
		return 0;
	}
	else
	{
		temp = n % 8;  
		//��ջ
		push_stack(sp, temp);
		n = n / 8;   
		convert(sp,n); 
		//��ջ
		printf("%d", pop_stack(sp));
	}
	return 0;
}
int main04()
{
	printf("���������֣�\n");
	ss* p = changed_stack();
	int a = 0;
	scanf("%d", &a);
	convert(p,a);
	//printf("%d", a);
	system("pause");
	return 0;
}
